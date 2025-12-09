#include "GameManager.h"
#include "Pastry.h"
#include "Beverage.h"

#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics.hpp>

GameManager::GameManager(Player& player_, std::vector<std::unique_ptr<Item>> items_, std::vector<Delivery> deliveries_)
         : player(player_), items(std::move(items_)), deliveries(std::move(deliveries_)) {
    deliveryRunning.resize(items.size(), false);
}

GameManager::GameManager(const GameManager& other)
    : player(other.player),
      deliveries(other.deliveries),
      deliveryRunning(other.deliveryRunning) {

    items.reserve(other.items.size());
    for (const auto& item : other.items) {
        items.push_back(std::unique_ptr<Item>(item->clone()));
    }
}

GameManager::~GameManager(){
    stopAllDeliveries();
    std::cout << "GameManager destroyed!\n";
}

GameManager& GameManager::operator=(const GameManager& other) {
    if (this != &other) {
        player = other.player;
        deliveries = other.deliveries;
        deliveryRunning = other.deliveryRunning;

        items.clear();
        items.reserve(other.items.size());
        for (const auto& item : other.items) {
            items.push_back(std::unique_ptr<Item>(item->clone()));
        }
    }
    return *this;
}

std::ostream &operator<<(std::ostream &ostream, const GameManager &manager) {
    ostream << "=== Game Manager ===\n";
    ostream << "Player: " << manager.player << " RON\n";
    ostream << "Items:\n";
    for (const auto& item : manager.items)
        ostream << "  " << *item;
    return ostream;
}

// Game logic

void GameManager::runDeliveryLoop(Item &item, std::size_t index) {
    std::thread([this, &item, index]() {
        sf::Clock clock;

        while (index < deliveryRunning.size() && deliveryRunning[index]) {
            if (clock.getElapsedTime() >= item.getDuration()) {
                player.setMoney(player.getMoney() + item.getBaseIncome());
                clock.restart();
            }

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }).detach();
}

void GameManager::sell(const Item &item) const {
    player.setMoney(player.getMoney() + item.getBaseIncome());
}

void GameManager::upgrade(Item &item) const {
    if (player.getMoney() >= item.getUpgradeCost()) {
        player.setMoney(player.getMoney() - item.getUpgradeCost());
        item.upgrade();
    }
}

void GameManager::applyAllBeverageEffects() const {
    for (auto& item : items) {
        if (const auto* bev = dynamic_cast<Beverage*>(item.get())) {

            if (bev->getTarget() == "ALL") {
                for (auto& targetPtr : items)
                    bev->applyToOne(*targetPtr);
            }
            else {
                for (auto& targetPtr : items)
                    if (targetPtr->getName() == bev->getTarget())
                        bev->applyToOne(*targetPtr);
            }
        }
    }
}


void GameManager::applyBeverageToItem(const Beverage& bev, Item& target) {
    bev.applyToOne(target);
}

void GameManager::startDelivery(Item &item, const Delivery &delivery, int index) {
    if (index < 0 || static_cast<std::size_t>(index) >= deliveryRunning.size()) return;

    if (!deliveryRunning[static_cast<std::size_t>(index)] && delivery.canUnlock(player)) {
        player.setMoney(player.getMoney() - delivery.getUnlockCost());
        deliveryRunning[static_cast<std::size_t>(index)] = true;
        runDeliveryLoop(item, static_cast<std::size_t>(index));
        std::cout << "Automation purchased for " << item.getName() << "!\n";
    }
}

void GameManager::stopAllDeliveries() {
    for (auto && i : deliveryRunning) {
        i = false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

GameManager GameManager::loadFromFile(const std::string &fileName, Player &player) {

    std::ifstream file(fileName);
    if (!file.is_open())
        throw std::runtime_error("Error: Unable to open file " + fileName);

    std::vector<std::unique_ptr<Item>> items;
    std::vector<Delivery> deliveries;

    std::string line;

    std::string type = "Pastry";
    std::string name, courierName;
    double baseIncome = 0, upgradeCost = 0, multiplier = 0;
    double unlockCost = 0, unlockDeliveryCost = 0;
    double durationSec = 2.0;

    std::vector<BeverageEffect> beverageEffects;
    std::string targetName = "ALL";

    auto reset_buffers = [&]() {
        type = "Pastry";
        name.clear();
        courierName.clear();
        baseIncome = upgradeCost = multiplier = unlockCost = unlockDeliveryCost = 0;
        durationSec = 2.0;
        beverageEffects.clear();
        targetName = "ALL";
    };

    auto create_and_add_item = [&]() {
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
        deliveries.emplace_back(courierName, unlockDeliveryCost);

        reset_buffers();
    };

    while (std::getline(file, line)) {
        if (line.empty()) {
            if (!name.empty()) create_and_add_item();
            continue;
        }

        std::istringstream iss(line);
        std::string key;

        if (std::getline(iss, key, ':')) {

            std::string value;
            std::getline(iss, value);
            if (!value.empty() && value[0] == ' ')
                value.erase(0, 1);

            if (key == "type") type = value;
            else if (key == "name") name = value;
            else if (key == "courierName") courierName = value;
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
                while (ev >> eType >> eValue) {
                    beverageEffects.push_back({ eType, eValue });
                }
            }

            else if (key == "target") {
                targetName = value;
            }
        }
    }

    if (!name.empty()) create_and_add_item();

    std::cout << "Loaded " << items.size() << " items.\n";

    GameManager gm(player, std::move(items), std::move(deliveries));
    gm.applyAllBeverageEffects();
    return gm;
}


void GameManager::saveGame() const {
    std::ofstream file("resources/savegame.txt");
    if (!file.is_open()) return;

    file << player.getMoney() << "\n";
    file << items.size() << "\n";

    for (std::size_t i = 0; i < items.size(); ++i) {

        if (auto bev = dynamic_cast<Beverage*>(items[i].get())) {
            file << "Beverage\n";
            file << bev->getName() << "\n";

            const auto& effects = bev->getEffects();
            file << effects.size() << "\n";
            for (const auto& e : effects)
                file << e.type << " " << e.value << " ";
            file << "\n";

            file << bev->getTarget() << "\n";
        }
        else if (auto pastry = dynamic_cast<Pastry*>(items[i].get())) {
            file << "Pastry\n";
            file << pastry->getName() << "\n";
            file << pastry->getBaseIncome() << "\n";
            file << pastry->getUpgradeCost() << "\n";
        }

        file << deliveryRunning[i] << "\n";
    }

    std::cout << "Game saved.\n";
}


bool GameManager::loadSavedGame() {

    std::ifstream file("resources/savegame.txt");
    if (!file.is_open()) return false;

    try {
        double savedMoney;
        file >> savedMoney;
        player.setMoney(savedMoney);

        int count;
        file >> count;

        for (std::size_t i = 0; i < items.size() && i < static_cast<std::size_t>(count); i++) {

            std::string type;
            file >> type;

            std::string tempName;
            file >> tempName;

            if (type == "Pastry") {

                double savedIncome, savedUpCost;
                bool running;
                file >> savedIncome >> savedUpCost >> running;

                items[i]->setBaseIncome(savedIncome);
                items[i]->setUpgradeCost(savedUpCost);
                deliveryRunning[i] = running;
            }

            else if (type == "Beverage") {

                int nEffects;
                file >> nEffects;

                std::vector<BeverageEffect> effects;
                effects.reserve(nEffects);

                for (int j = 0; j < nEffects; j++) {
                    std::string eType;
                    double eVal;
                    file >> eType >> eVal;
                    effects.push_back({ eType, eVal });
                }

                std::string target;
                file >> target;

                bool running;
                file >> running;

                if (auto bev = dynamic_cast<Beverage*>(items[i].get())) {
                    bev->setEffects(effects);
                    bev->setTarget(target);
                }

                deliveryRunning[i] = running;
            }

            if (deliveryRunning[i])
                runDeliveryLoop(*items[i], i);
        }

        applyAllBeverageEffects();
        return true;
    }
    catch (...) {
        return false;
    }
}

std::vector<std::unique_ptr<Item>>& GameManager::getItems() {
    return items;
}

std::vector<Delivery>& GameManager::getDelivery() {
    return deliveries;
}
