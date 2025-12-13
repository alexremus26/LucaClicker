#include "GameManager.h"
#include "Pastry.h"
#include "Beverage.h"

#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>
#include <ranges>
#include <SFML/System/Clock.hpp>

GameManager::GameManager(Player& player_,
                         std::vector<std::unique_ptr<Item>> items_,
                         std::vector<Delivery> deliveries_)
    : player(player_),
      items(std::move(items_)),
      deliveries(std::move(deliveries_))
{
    deliveryRunning.resize(items.size(), false);
    sellingRunning.resize(items.size(), false);
    itemUnlocked.resize(items.size(), false);
    sellProgress.resize(items.size(), 0.f);

    if (!items.empty())
        itemUnlocked[0] = true;
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
}

GameManager& GameManager::operator=(const GameManager& other) {
    if (this != &other) {
        player = other.player;
        deliveries = other.deliveries;
        deliveryRunning = other.deliveryRunning;
        itemUnlocked = other.itemUnlocked;

        items.clear();
        items.reserve(other.items.size());
        for (const auto& item : other.items)
            items.push_back(std::unique_ptr<Item>(item->clone()));
    }
    return *this;
}

GameManager::~GameManager() {
    stopAllDeliveries();
    std::cout << "GameManager destroyed!\n";
}

std::ostream& operator<<(std::ostream& ostream, const GameManager& manager)
{
    ostream << "=== Game Manager ===\n";
    ostream << "Player money: " << manager.player.getMoney() << "\n";
    ostream << "Items:\n";

    for (const auto& item : manager.items)
        ostream << "  " << *item << "\n";

    return ostream;
}

int GameManager::unlockItem(const std::size_t index) {

    if (itemUnlocked[index])
        return 1; // already unlocked

    const double cost = items[index]->getUnlockCost();
    if (!player.enoughMoney(cost))
        return 2; // not enough money

    if (index >= items.size())
        return 3; // invalid index
    if (!player.tryPay(items[index]->getUnlockCost()))
        return 2;

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
                const double income = platform.computeIncome(item);
                player.earn(income);
                clock.restart();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
    }).detach();
}
void GameManager::runSellingLoop(Item& item, std::size_t index)
{
    if (sellingRunning[index])
        return;

    sellingRunning[index] = true;

    std::thread([this, &item, index]() {
        const sf::Clock clock;

        sellProgress[index] = 0.f;

        const sf::Time duration = item.getDuration();

        while (clock.getElapsedTime() < duration)
        {
            float p = clock.getElapsedTime().asSeconds() / duration.asSeconds();
            sellProgress[index] = std::min(1.f, p);

            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }

        sell(item);
        sellProgress[index] = 0.f;
        sellingRunning[index] = false;

    }).detach();
}


void GameManager::sell(const Item& item) const {
    player.earn(item.getBaseIncome());
}

void GameManager::upgrade(Item& item) const {
    if (player.tryPay(item.getUpgradeCost())) {
        item.upgrade();
    }
}


float GameManager::getSellProgress(std::size_t index) const {
    return (index < sellProgress.size()) ? sellProgress[index] : 0.f;
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
        player.tryPay(delivery.getUnlockCost()))
    {
            deliveryRunning[index] = true;
        }

        runDeliveryLoop(item, index);
        std::cout << "Automation purchased for " << item.getName() << "!\n";
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
        durationSec = 0;
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
        // silence warnings
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
        const std::size_t pos = line.find(':');
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

bool GameManager::isSelling(const std::size_t index) const {
    return index < sellingRunning.size() && sellingRunning[index];
}

std::vector<std::unique_ptr<Item>>& GameManager::getItems() {
    return items;
}

std::vector<Delivery>& GameManager::getDelivery() {
    return deliveries;
}
