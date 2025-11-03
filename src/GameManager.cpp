#include "GameManager.h"
#include <iostream>
#include <thread>
#include <fstream>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics.hpp>

GameManager::GameManager(Player& player_, std::vector<FoodItem> foodItem_, std::vector<Delivery> deliveries_)
         : player(player_), foodItems(std::move(foodItem_)), deliveries(std::move(deliveries_)) {
    deliveryRunning.resize(foodItems.size(), false);
}

GameManager::GameManager(const GameManager& gameManager)
    : player(gameManager.player), foodItems(gameManager.foodItems), deliveries(gameManager.deliveries),
      deliveryRunning(gameManager.deliveryRunning) {}

GameManager::~GameManager(){
    stopAllDeliveries();
    std::cout<<"GameManager a fost distrus! \n";
}

GameManager& GameManager::operator=(const GameManager& manager) {
    if (this != &manager) {
        player = manager.player;
        foodItems = manager.foodItems;
        deliveries = manager.deliveries;
        deliveryRunning = manager.deliveryRunning;
    }
    return *this;
}

std::ostream &operator<<(std::ostream &ostream, const GameManager &manager) {
    ostream << "=== Game Manager ===\n";
    ostream << "Player: " << manager.player << " RON\n";
    ostream << "Food items:\n";
    for (const auto& food : manager.foodItems)
        ostream << "  " << food;
    return ostream;
}

void GameManager::runDeliveryLoop(FoodItem &food, Delivery &delivery, int index) {
    std::thread([this, &food, &delivery, index]() {
        sf::Clock clock;
        while (deliveryRunning[index]) {
            // Add income at the determined interval
            if (clock.getElapsedTime().asSeconds() >= delivery.getTimeInterval().asSeconds()) {
                player.setMoney(player.getMoney() + food.getBaseIncome());
                clock.restart();
            }
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(50ms); // Prevent CPU overuse
        }
    }).detach();
}

GameManager GameManager::loadFromFile(const std::string &fileName, Player &player) {
    std::ifstream file(fileName);
    if (!file.is_open())
        throw std::runtime_error("Error: Unable to open file " + fileName);

    std::vector<FoodItem> foodItems;
    std::vector<Delivery> deliveries;

    std::string line;

    // Temporary variables for parsing
    std::string foodName, courierName;
    double baseIncome = 0, upgradeCost = 0, upgradeMultiplier = 0;
    double incomeMultiplier = 0, unlockFoodCost = 0, unlockDeliveryCost = 0;

    // Reset function for parsing new items
    auto reset_fields = [&]() {
        foodName.clear();
        courierName.clear();
        baseIncome = upgradeCost = upgradeMultiplier = incomeMultiplier = unlockFoodCost = unlockDeliveryCost = 0;
    };

    // Parse each line of the file
    while (std::getline(file, line)) {
        // Empty line indicates end of current food item definition
        if (line.empty()) {
            if (!foodName.empty()) {
                // Create food item and delivery from parsed data
                foodItems.emplace_back(
                    foodName,
                    baseIncome,
                    upgradeCost,
                    incomeMultiplier,
                    upgradeMultiplier,
                    unlockFoodCost
                );

                deliveries.emplace_back(courierName,unlockDeliveryCost);

                reset_fields();
            }
            continue;
        }

        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, ':')) {
            std::string value;
            std::getline(iss, value);

            // Remove space if present
            if (!value.empty() && value[0] == ' ')
                value.erase(0, 1);

            // Parse different property types
            if (key == "foodName") foodName = value;
            else if (key == "baseIncome") baseIncome = std::stod(value);
            else if (key == "upgradeCost") upgradeCost = std::stod(value);
            else if (key == "upgradeMultiplier") upgradeMultiplier = std::stod(value);
            else if (key == "incomeMultiplier") incomeMultiplier = std::stod(value);
            else if (key == "unlockFoodCost") unlockFoodCost = std::stod(value);
            else if (key == "courierName") courierName = value;
            else if (key == "unlockDeliveryCost") unlockDeliveryCost = std::stod(value);
        }
    }

    // Handle last item if file doesn't end with empty line
    if (!foodName.empty()) {
        foodItems.emplace_back(
            foodName,
            baseIncome,
            upgradeCost,
            incomeMultiplier,
            upgradeMultiplier,
            unlockFoodCost
        );

        deliveries.emplace_back(courierName, unlockDeliveryCost);
    }

        // Display loaded items for verification
        std::cout << "Loaded " << foodItems.size() << " food items and couriers:\n";
        for (size_t i = 0; i < foodItems.size(); ++i) {
            const auto& food = foodItems[i];
            const auto& delivery = deliveries[i];

            std::cout << "=======================================================\n";
            std::cout << "Food Name: " << food.getFoodName() << "\n"
                      << "  Base Income: " << food.getBaseIncome() << "\n"
                      << "  Upgrade Cost: " << food.getUpgradeCost() << "\n"
                      << "  Unlock Food Cost: " << food.getUnlockCost() << "\n"
                      << "  Delivery Unlock Cost: " << delivery.getUnlockCost() << "\n"
                      << "  Delivery Interval: " << delivery.getTimeInterval().asSeconds() << "s\n";
        }
             std::cout<< "=======================================================\n";

    return { player, std::move(foodItems), std::move(deliveries) };
}

void GameManager::sell(const FoodItem &foodItem) const {
    player.setMoney(player.getMoney() + foodItem.getBaseIncome());
}

void GameManager::upgrade(FoodItem &foodItem) const {
    if (player.getMoney() >= foodItem.getUpgradeCost()) {
        player.setMoney(player.getMoney() - foodItem.getUpgradeCost());
        foodItem.update();
    }
}

void GameManager::startDelivery(FoodItem &foodItem, Delivery &delivery, const int index) {
    if (!deliveryRunning[index-1] && delivery.canUnlock(player)) {
        player.setMoney(player.getMoney() - delivery.getUnlockCost());
        deliveryRunning[index-1] = true;
        runDeliveryLoop(foodItem, delivery, index-1);
    }
}

void GameManager::stopAllDeliveries() {
    for (size_t i = 0; i < deliveryRunning.size(); ++i) {
        deliveryRunning[i] = false;
    }
}

void GameManager::saveGame() const {
    std::ofstream file("resources/savegame.txt");
    if (!file.is_open()) {
        file.open("savegame.txt");
        if (!file.is_open()) {
            std::cerr << "Warning: Could not save game progress\n";
            return;
        }
    }

    file << player.getMoney() << "\n";
    file << foodItems.size() << "\n";
    for (size_t i = 0; i < foodItems.size(); ++i) {
        const auto& food = foodItems[i];
        file << food.getFoodName() << "\n";
        file << food.getBaseIncome() << "\n";
        file << food.getUpgradeCost() << "\n";
        file << deliveryRunning[i] << "\n";  // Save delivery state per food item
    }

    file.close();
    std::cout << "Game progress saved automatically\n";
}

bool GameManager::loadSavedGame() {
    std::ifstream file("resources/savegame.txt");
    if (!file.is_open()) {
        file.open("savegame.txt");
        if (!file.is_open()) {
            return false;
        }
    }
    try {
        double savedMoney;
        file >> savedMoney;
        player.setMoney(savedMoney);
        std::cout << "Loaded saved game with " << savedMoney << " RON\n";

        int foodCount;
        file >> foodCount;
        bool deliveryState;

        for (size_t i = 0; i < foodItems.size() && i < static_cast<size_t>(foodCount); ++i) {
            auto& food = foodItems[i];
            std::string foodName;
            double baseIncome, upgradeCost;

            file >> foodName;
            file >> baseIncome;
            file >> upgradeCost;
            file >> deliveryState;

            food.setBaseIncome(baseIncome);
            food.setUpgradeCost(upgradeCost);
            deliveryRunning[i] = deliveryState;

            // Restart delivery if it was running
            if (deliveryRunning[i]) {
                runDeliveryLoop(food, deliveries[i], i);
            }
        }

        file.close();
        return true;
    } catch (...) {
        std::cerr << "Error reading save file\n";
        return false;
    }
}

std::vector<FoodItem> &GameManager::getFoods() {
    return foodItems;
}

std::vector<Delivery> &GameManager::getDelivery() {
    return deliveries;
}
