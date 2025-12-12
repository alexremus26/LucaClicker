#include "GameManager.h"
#include "Pastry.h"
#include "Beverage.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <ranges>
#include <algorithm>
#include <thread>
#include <SFML/System/Clock.hpp>

GameManager::GameManager(Player& player_,
                         std::vector<std::unique_ptr<Item>> items_,
                         std::vector<Delivery> deliveries_)
    : player(player_),
      items(std::move(items_)),
      deliveries(std::move(deliveries_))
{
    initializeRuntimeState();
}

GameManager::GameManager(const GameManager& other)
    : player(other.player),
      deliveries(other.deliveries),
      deliveryRunning(other.deliveryRunning),
      itemUnlocked(other.itemUnlocked)
{
    items.reserve(other.items.size());
    for (const auto& item : other.items)
        items.push_back(std::unique_ptr<Item>(item->clone()));

    initializeRuntimeState();
}

GameManager& GameManager::operator=(const GameManager& other) {
    if (this != &other) {
        stopSellingThreads();

        player = other.player;
        deliveries = other.deliveries;
        deliveryRunning = other.deliveryRunning;
        itemUnlocked = other.itemUnlocked;

        items.clear();
        items.reserve(other.items.size());
        for (const auto& item : other.items)
            items.push_back(std::unique_ptr<Item>(item->clone()));

        initializeRuntimeState();
    }
    return *this;
}

GameManager::~GameManager() {
    stopSellingThreads();
    stopAllDeliveries();
    std::cout << "GameManager destroyed!\n";
}

void GameManager::initializeRuntimeState() {
    deliveryRunning.resize(items.size(), false);
    itemUnlocked.resize(items.size(), false);
    sellProgress.clear();
    sellProgress.resize(items.size());

    sellingActive.clear();
    sellingActive.resize(items.size());
    stopSellingWorkers.clear();
    stopSellingWorkers.resize(items.size());
    pendingSales.clear();
    pendingSales.resize(items.size());

    for (std::size_t i = 0; i < items.size(); ++i) {
        sellingActive[i] = false;
        stopSellingWorkers[i] = false;
        pendingSales[i] = 0;
    }

    if (!items.empty())
        itemUnlocked[0] = true;

    for (std::size_t i = 0; i < items.size(); ++i) {
        sellProgress[i] = 0.f;
    }

    sellingWorkers.clear();
    sellingWorkers.reserve(items.size());

    for (std::size_t i = 0; i < items.size(); ++i) {
        sellingWorkers.emplace_back([this, i]() { sellingWorker(i); });
    }
}

void GameManager::stopSellingThreads() {
    if (sellingWorkers.empty())
        return;

    for (auto& stopFlag : stopSellingWorkers)
        stopFlag = true;

    for (auto& worker : sellingWorkers) {
        if (worker.joinable())
            worker.join();
    }

    sellingWorkers.clear();
}

std::ostream& operator<<(std::ostream& os, const GameManager& manager)
{
    os << "=== Game Manager ===\n";
    os << "Player money: " << manager.player.getMoney() << "\n";
    os << "Items:\n";

    for (const auto& item : manager.items)
        os << "  " << *item << "\n";

    return os;
}

int GameManager::unlockItem(const std::size_t index) {

    if (itemUnlocked[index])
        return 1; // already unlocked

    const double cost = items[index]->getUnlockCost();
    if (player.getMoney() < cost)
        return 2; // not enough money

    if (index >= items.size())
        return 3; // invalid index

    player.setMoney(player.getMoney() - cost);
    itemUnlocked[index] = true;

    return 0; // success
}

bool GameManager::isUnlocked(const std::size_t index) const {
    return index < itemUnlocked.size() && itemUnlocked[index];
}

const std::vector<bool>& GameManager::getUnlocked() const {
    return itemUnlocked;
}

void GameManager::runDeliveryLoop(Item& item, std::size_t index) {
    std::thread([this, &item, index]() {

        sf::Clock clock;

        while (index < deliveryRunning.size() && deliveryRunning[index]) {
            Delivery& delivery = deliveries[index];

            if (DeliveryPlatform& platform = delivery.getPlatform();
                clock.getElapsedTime() >= platform.computeSpeed(item))
            {
                double income = platform.computeIncome(item);
                player.setMoney(player.getMoney() + income);
                clock.restart();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
    }).detach();
}
void GameManager::sellingWorker(std::size_t index) {
    while (index < items.size()) {
        if (stopSellingWorkers[index])
            break;

        if (pendingSales[index] > 0 && !sellingActive[index]) {
            pendingSales[index]--;
            sellingActive[index] = true;
            sellProgress[index] = 0.f;

            sf::Clock clock;
            const sf::Time duration = items[index]->getDuration();

            while (clock.getElapsedTime() < duration && !stopSellingWorkers[index]) {
                const float t = clock.getElapsedTime().asSeconds();
                const float total = duration.asSeconds();

                sellProgress[index] = std::min(t / total, 1.f);
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
            }

            if (!stopSellingWorkers[index]) {
                sell(*items[index]);
                sellProgress[index] = 1.f;
                std::this_thread::sleep_for(std::chrono::milliseconds(80));
            }

            sellProgress[index] = 0.f;
            sellingActive[index] = false;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
}

bool GameManager::runSellingLoop(Item &item, std::size_t index) {
    (void)item;
    if (index >= items.size())
        return false;

    if (sellingActive[index] || pendingSales[index] > 0)
        return false;

    pendingSales[index]++;
    return true;
}


void GameManager::sell(const Item& item) const {
    player.setMoney(player.getMoney() + item.getBaseIncome());
}

void GameManager::upgrade(Item& item) const {
    if (player.getMoney() >= item.getUpgradeCost()) {
        player.setMoney(player.getMoney() - item.getUpgradeCost());
        item.upgrade();
    }
}

[[nodiscard]] float GameManager::getSellProgress(const std::size_t index) const {
    if (index >= sellProgress.size())
        return 0.f;

    return sellProgress[index];
}

void GameManager::applyAllBeverageEffects() const {
    for (auto& item : items) {
        if (const auto* bev = dynamic_cast<Beverage*>(item.get())) {

            if (bev->getTarget() == "ALL") {
                for (auto& target : items)
                    bev->applyToOne(*target);
            } else {
                for (auto& target : items)
                    if (target->getName() == bev->getTarget())
                        bev->applyToOne(*target);
            }
        }
    }
}

void GameManager::applyBeverageToItem(const Beverage& bev, Item& target) {
    bev.applyToOne(target);
}

void GameManager::startDelivery(Item& item, const Delivery& delivery, int index) {
    if (index < 0 || static_cast<std::size_t>(index) >= deliveryRunning.size())
        return;

    if (!deliveryRunning[index] &&
        player.getMoney() >= delivery.getUnlockCost())
    {
        player.setMoney(player.getMoney() - delivery.getUnlockCost());
        deliveryRunning[index] = true;

        runDeliveryLoop(item, index);
        std::cout << "Automation purchased for " << item.getName() << "!\n";
    }
}

void GameManager::stopAllDeliveries() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

GameManager GameManager::loadFromFile(const std::string& fileName, Player& player) {
    std::ifstream file(fileName);
    if (!file.is_open())
        throw std::runtime_error("Error opening " + fileName);

    std::vector<std::unique_ptr<Item>> items;
    std::vector<Delivery> deliveries;

    std::string line;

    std::string type = "Pastry";
    std::string name, deliveryName;
    double baseIncome = 0, upgradeCost = 0, multiplier = 0;
    double unlockCost = 0, unlockDeliveryCost = 0;
    double durationSec = 2.0;

    std::vector<BeverageEffect> beverageEffects;
    std::string targetName = "ALL";

    auto reset_buffers = [&]() {
        type = "Pastry";
        name.clear();
        deliveryName.clear();
        baseIncome = upgradeCost = multiplier = unlockCost = unlockDeliveryCost = 0;
        durationSec = 2.0;
        beverageEffects.clear();
        targetName = "ALL";
    };

    auto add_item = [&]() {
        std::unique_ptr<Item> newItem;

        if (type == "Pastry") {
            newItem = std::make_unique<Pastry>(
                name, multiplier, unlockCost,
                baseIncome, upgradeCost,
                sf::seconds(static_cast<float>(durationSec))
            );
        }
        else if (type == "Beverage") {
            newItem = std::make_unique<Beverage>(
                name, multiplier, unlockCost,
                beverageEffects,
                targetName
            );
        }

        items.push_back(std::move(newItem));
        deliveries.emplace_back(deliveryName, unlockDeliveryCost);

        reset_buffers();
    };

    while (std::getline(file, line)) {

        if (line.empty()) {
            if (!name.empty()) add_item();
            continue;
        }

        std::istringstream iss(line);
        std::string key;

        if (!std::getline(iss, key, ':'))
            continue;

        std::string value;
        std::getline(iss, value);
        if (!value.empty() && value[0] == ' ')
            value.erase(0, 1);

        if (key == "type") type = value;
        else if (key == "name") name = value;
        else if (key == "deliveryName") deliveryName = value;
        else if (key == "baseIncome") baseIncome = std::stod(value);
        else if (key == "upgradeCost") upgradeCost = std::stod(value);
        else if (key == "multiplier") multiplier = std::stod(value);
        else if (key == "unlockCost") unlockCost = std::stod(value);
        else if (key == "unlockDeliveryCost") unlockDeliveryCost = std::stod(value);
        else if (key == "duration") durationSec = std::stod(value);

        else if (key == "effects") {
            beverageEffects.clear();
            std::istringstream ev(value);
            std::string eType;
            double eValue;
            while (ev >> eType >> eValue)
                beverageEffects.emplace_back(eType, eValue);
        }

        else if (key == "target") {
            targetName = value;
        }
    }

    if (!name.empty()) {
        add_item();
    } else {
        // silence unused variable warnings
        (void)type; (void)name; (void)deliveryName;
        (void)baseIncome; (void)upgradeCost; (void)multiplier;
        (void)unlockCost; (void)unlockDeliveryCost;
        (void)durationSec;
    }

    std::cout << "Loaded " << items.size() << " items.\n";

return { player, std::move(items), std::move(deliveries) };
}

void GameManager::saveGame() const {
    std::ofstream file("resources/savegame.txt");
    if (!file.is_open()) return;

    file << "money: " << player.getMoney() << "\n";
    file << "items: " << items.size() << "\n\n";

    for (std::size_t i = 0; i < items.size(); ++i) {

        file << "item:\n";
        file << "unlocked: " << itemUnlocked[i] << "\n";

        if (const auto* bev = dynamic_cast<Beverage*>(items[i].get())) {
            file << "type: Beverage\n";
            file << "name: " << bev->getName() << "\n";

            const auto& effects = bev->getEffects();
            file << "effectsCount: " << effects.size() << "\n";
            file << "effects: ";

            for (const auto& [type, value] : effects)
                file << type << " " << value << " ";

            file << "\n";
            file << "target: " << bev->getTarget() << "\n";
        }
        else if (const auto* pastry = dynamic_cast<Pastry*>(items[i].get())) {
            file << "type: Pastry\n";
            file << "name: " << pastry->getName() << "\n";
            file << "baseIncome: " << pastry->getBaseIncome() << "\n";
            file << "upgradeCost: " << pastry->getUpgradeCost() << "\n";
        }

        file << "deliveryRunning: " << deliveryRunning[i] << "\n\n";
    }

    std::cout << "Game saved.\n";
}


bool GameManager::loadSavedGame() {
    std::ifstream file("resources/savegame.txt");
    if (!file.is_open()) return false;

    std::string line;

    auto getKV = [&](std::string& key, std::string& value) {
        std::getline(file, line);
        std::size_t pos = line.find(':');
        if (pos == std::string::npos) return false;

        key = line.substr(0, pos);
        value = line.substr(pos + 2);
        return true;
    };

    {
        std::string key, value;
        if (!getKV(key, value) || key != "money") return false;
        player.setMoney(std::stod(value));
    }

    {
        std::string key, value;
        if (!getKV(key, value) || key != "items") return false;
    }

    std::size_t index = 0;

    while (std::getline(file, line)) {

        if (line != "item:")
            continue;

        bool unlocked = false;
        bool running = false;

        std::string type;
        std::string name;
        double baseIncome = 0, upgradeCost = 0;
        int effectsCount = 0;
        std::vector<BeverageEffect> effects;
        std::string target;

        while (std::getline(file, line) && !line.empty()) {

            std::size_t pos = line.find(':');
            if (pos == std::string::npos) continue;

            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);

            if (key == "unlocked") unlocked = std::stoi(value);
            else if (key == "type") type = value;
            else if (key == "name") name = value;
            else if (key == "baseIncome") baseIncome = std::stod(value);
            else if (key == "upgradeCost") upgradeCost = std::stod(value);
            else if (key == "effectsCount") {
                effectsCount = std::stoi(value);
                effects.reserve(effectsCount);
            }
            else if (key == "effects") {
                std::istringstream ev(value);
                std::string t;
                double val;
                while (ev >> t >> val)
                    effects.emplace_back(t, val);
            }
            else if (key == "target") target = value;
            else if (key == "deliveryRunning") running = std::stoi(value);
        }

        if (index < items.size()) {

            itemUnlocked[index] = unlocked;

            if (auto* pastry = dynamic_cast<Pastry*>(items[index].get())) {
                pastry->setBaseIncome(baseIncome);
                pastry->setUpgradeCost(upgradeCost);
            }
            else if (auto* bev = dynamic_cast<Beverage*>(items[index].get())) {
                if (!effects.empty()) bev->setEffects(effects);
                if (!target.empty()) bev->setTarget(target);
            }

            deliveryRunning[index] = running;
            if (running)
                runDeliveryLoop(*items[index], index);
        }

        (void)type;
        (void)name;
        (void)effectsCount;

        ++index;
    }

    return true;
}

std::vector<std::unique_ptr<Item>>& GameManager::getItems() {
    return items;
}

std::vector<Delivery>& GameManager::getDelivery() {
    return deliveries;
}
