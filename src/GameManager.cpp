#include "GameManager.h"
#include "Pastry.h"

#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>
#include <memory> // For std::unique_ptr
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics.hpp>

// ==========================================
// CONSTRUCTORS & DESTRUCTORS
// ==========================================

GameManager::GameManager(Player& player_, std::vector<std::unique_ptr<Item>> items_, std::vector<Delivery> deliveries_)
         : player(player_), items(std::move(items_)), deliveries(std::move(deliveries_)) {
    deliveryRunning.resize(items.size(), false);
}

// Copy Constructor: DEEP COPY needed for pointers
GameManager::GameManager(const GameManager& other)
    : player(other.player),
      deliveries(other.deliveries),
      deliveryRunning(other.deliveryRunning) {

    items.reserve(other.items.size());
    for (const auto& item : other.items) {
        // CALLS THE VIRTUAL CLONE METHOD (Polymorphism)
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

        // Clear old pointers
        items.clear();

        // Deep copy new ones
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
        ostream << "  " << *item; // Dereference pointer to print
    return ostream;
}

// ==========================================
// GAME LOGIC
// ==========================================

void GameManager::runDeliveryLoop(Item &item, Delivery &delivery, int index) {
    std::thread([this, &item, &delivery, index]() {
        sf::Clock clock;

        // LOOP CONDITION: Checks if the "Delivery" (Automation) is active.
        // If deliveryRunning[index] becomes false, the automation stops.
        while (index < deliveryRunning.size() && deliveryRunning[index]) {

            // TIMING CONDITION: Uses the ITEM'S duration.
            // Example: A Croissant takes 2.0 seconds to bake/sell.
            if (clock.getElapsedTime() >= item.getDuration()) {

                // REVENUE: Uses the ITEM'S revenue calculation.
                player.setMoney(player.getMoney() + item.calculateRevenue());

                // Reset the timer for the next product
                clock.restart();
            }

            // Sleep to save CPU usage
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }).detach();
}
void GameManager::sell(const Item &item) const {
    player.setMoney(player.getMoney() + item.calculateRevenue());
}

void GameManager::upgrade(Item &item) const {
    if (player.getMoney() >= item.getUpgradeCost()) {
        player.setMoney(player.getMoney() - item.getUpgradeCost());
        item.upgrade(); // Polymorphic call
    }
}

void GameManager::startDelivery(Item &item, Delivery &delivery, const int index) {
    // Safety check for index bounds
    if (index < 0 || index >= deliveryRunning.size()) return;

    // 1. Check if automation is NOT already running
    // 2. Check if player has enough money to buy this specific Automation/Courier
    if (!deliveryRunning[index] && delivery.canUnlock(player)) {

        // Deduct the cost of the Automation (One-time purchase)
        player.setMoney(player.getMoney() - delivery.getUnlockCost());

        // Turn the switch ON
        deliveryRunning[index] = true;

        // Spin up the thread that does the work for us
        runDeliveryLoop(item, delivery, index);

        std::cout << "Automation purchased for " << item.getName() << "!\n";
    }
}

void GameManager::stopAllDeliveries() {
    for (size_t i = 0; i < deliveryRunning.size(); ++i) {
        deliveryRunning[i] = false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// ==========================================
// FILE I/O (FACTORY PATTERN)
// ==========================================

GameManager GameManager::loadFromFile(const std::string &fileName, Player &player) {
    std::ifstream file(fileName);
    if (!file.is_open())
        throw std::runtime_error("Error: Unable to open file " + fileName);

    std::vector<std::unique_ptr<Item>> items;
    std::vector<Delivery> deliveries;

    std::string line;

    // Data buffers
    std::string type = "Pastry"; // Default type
    std::string name, courierName;
    double baseIncome = 0, upgradeCost = 0, multiplier = 0;
    double unlockCost = 0, unlockDeliveryCost = 0;
    double durationSec = 2.0;

    // Lambda to create objects based on 'type'
    auto create_and_add_item = [&]() {
        std::unique_ptr<Item> newItem;

        if (type == "Pastry") {
            // Create a Pastry
            newItem = std::make_unique<Pastry>(
                name, baseIncome, upgradeCost, multiplier, unlockCost, sf::seconds(durationSec)
            );
        }
        // FUTURE EXPANSION:
        // else if (type == "Beverage") {
        //     newItem = std::make_unique<Beverage>(...args...);
        // }
        else {
            // Fallback to Pastry if unknown
            newItem = std::make_unique<Pastry>(
                 name, baseIncome, upgradeCost, multiplier, unlockCost, sf::seconds(durationSec)
            );
        }

        items.push_back(std::move(newItem));
        deliveries.emplace_back(courierName, unlockDeliveryCost);

        // Reset buffers
        name.clear(); courierName.clear(); type = "Pastry";
        baseIncome = upgradeCost = multiplier = unlockCost = unlockDeliveryCost = 0;
        durationSec = 2.0;
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
            if (!value.empty() && value[0] == ' ') value.erase(0, 1);

            if (key == "type") type = value; // Allows file to specify "Beverage"
            else if (key == "foodName" || key == "name") name = value;
            else if (key == "courierName") courierName = value;
            else if (key == "baseIncome") baseIncome = std::stod(value);
            else if (key == "upgradeCost") upgradeCost = std::stod(value);
            else if (key == "upgradeMultiplier" || key == "multiplier") multiplier = std::stod(value);
            else if (key == "unlockFoodCost" || key == "unlockCost") unlockCost = std::stod(value);
            else if (key == "unlockDeliveryCost") unlockDeliveryCost = std::stod(value);
            else if (key == "duration") durationSec = std::stod(value);
        }
    }

    if (!name.empty()) create_and_add_item();

    std::cout << "Loaded " << items.size() << " items.\n";
    return { player, std::move(items), std::move(deliveries) };
}

void GameManager::saveGame() const {
    std::ofstream file("resources/savegame.txt");
    if (!file.is_open()) return;

    file << player.getMoney() << "\n";
    file << items.size() << "\n";

    for (size_t i = 0; i < items.size(); ++i) {
        // NOTE: If you have different types, you might want to save the "Type" here too
        // file << "type: " << "Pastry" << "\n";

        file << items[i]->getName() << "\n";
        file << items[i]->getBaseIncome() << "\n";
        file << items[i]->getUpgradeCost() << "\n";
        file << deliveryRunning[i] << "\n";
    }

    file.close();
    std::cout << "Game progress saved automatically\n";
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

        for (size_t i = 0; i < items.size() && i < static_cast<size_t>(count); ++i) {
            std::string tempName;
            double savedIncome, savedUpCost;
            bool isRunning;

            // Simple reading (adjust if your save format changes)
            file >> tempName;
            file >> savedIncome;
            file >> savedUpCost;
            file >> isRunning;

            items[i]->setBaseIncome(savedIncome);
            items[i]->setUpgradeCost(savedUpCost);

            deliveryRunning[i] = isRunning;

            if (deliveryRunning[i]) {
                runDeliveryLoop(*items[i], deliveries[i], i);
            }
        }
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<std::unique_ptr<Item>>& GameManager::getItems() {
    return items;
}

std::vector<Delivery>& GameManager::getDelivery() {
    return deliveries;
}