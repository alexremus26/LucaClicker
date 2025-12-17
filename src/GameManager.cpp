#include "GameManager.h"
#include "GameExceptions.h"
#include "Pastry.h"
#include "Beverage.h"
#include "Sandwich.h"

#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>
#include <map>
#include <optional>
#include <SFML/System/Clock.hpp>
#include <tuple>

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
    progress.resize(items.size(), 0.f);

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

std::string GameManager::unlockItem(const std::size_t index) {
    if (index >= itemUnlocked.size()) {
        throw InvalidIndexException("Attempted to unlock item at invalid index " + std::to_string(index) + ".");
    }
    if (itemUnlocked[index])
        return "Item already unlocked!";

    const double cost = items[index]->getUnlockCost();
    if (!player.enoughMoney(cost))
        return "Not enough money!";

    itemUnlocked[index] = true;

    return "Unlocked item!";
}

bool GameManager::isUnlocked(const std::size_t index) const {
    return index < itemUnlocked.size() && itemUnlocked[index];
}


std::thread GameManager::runDeliveryLoop(Item& item, std::size_t index) {
    return std::thread([this, &item, index]() {
        const Delivery& delivery = deliveries[index];
        const DeliveryPlatform& platform = delivery.getPlatform();

        while (deliveryRunning[index]) {

            sellingRunning[index] = true;
            {
                const sf::Clock clock;
                const double speedMultiplier = combinedSpeedMultiplier();
                const sf::Time duration = platform.computeSpeed(item) / static_cast<float>(speedMultiplier);
                progress[index] = 0.f;

                while (clock.getElapsedTime() < duration) {

                    if (!deliveryRunning[index]) {
                        progress[index] = 0.f;
                        sellingRunning[index] = false;
                        return;
                    }

                    float p = clock.getElapsedTime().asSeconds() / duration.asSeconds();
                    progress[index] = std::min(1.f, p);
                    std::this_thread::sleep_for(std::chrono::milliseconds(40));
                }

                const auto [income, message] = platform.computeIncome(item);
                player.earn(income);
                if (!message.empty()) {
                    pushEventMessage(message);
                }

                progress[index] = 0.f;
            }
            sellingRunning[index] = false;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
}
void GameManager::runSellingLoop(Item& item, std::size_t index)
{
    if (deliveryRunning[index])
        return;

    if (sellingRunning[index])
        return;

    sellingRunning[index] = true;

    std::thread([this, &item, index]() {
        const sf::Clock clock;

        progress[index] = 0.f;

        const double speedMultiplier = combinedSpeedMultiplier();
        const sf::Time duration = item.getDuration() / static_cast<float>(speedMultiplier);

        while (clock.getElapsedTime() < duration)
        {
            float p = clock.getElapsedTime().asSeconds() / duration.asSeconds();
            progress[index] = std::min(1.f, p);

            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }

        sell(item);
        progress[index] = 0.f;
        sellingRunning[index] = false;

    }).detach();
}


void GameManager::sell(const Item& item) const {
    player.earn(item.sellPayout());
}

void GameManager::upgrade(Item& item) const{
    const double cost = item.getUpgradeCost();
    if (cost > 0 && player.tryPay(cost)) {
        item.upgrade();
    }
}


float GameManager::getProgress(const std::size_t index) const {
    return (index < progress.size()) ? progress[index] : 0.f;
}

void GameManager::startDelivery(Item& item, const Delivery& delivery, const int index) {
    if (index < 0 || static_cast<std::size_t>(index) >= deliveryRunning.size())
        throw InvalidIndexException("Delivery index " + std::to_string(index) + " is out of bounds.");

    if (sellingRunning[index])
        return;

    if (!deliveryRunning[index] &&
        player.tryPay(delivery.getUnlockCost()))
    {
            deliveryRunning[index] = true;
            deliveryThreads.emplace_back(runDeliveryLoop(item, index));
            std::cout << "Automation purchased for " << item.getName() << "!\n";
    }
}

void GameManager::stopAllDeliveries() {
    for (auto && i : deliveryRunning) {
        i = false;
    }

    for (std::thread& t : deliveryThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
    deliveryThreads.clear();
}

void GameManager::pushEventMessage(const std::string& message) {
    const std::lock_guard<std::mutex> lock(eventMutex);
    eventMessages.push(message);
}

std::string GameManager::popEventMessage() {
    const std::lock_guard<std::mutex> lock(eventMutex);
    if (eventMessages.empty()) {
        return "";
    }
    std::string message = eventMessages.front();
    eventMessages.pop();
    return message;
}

std::unique_ptr<Item> GameManager::createItemFromConfig(const std::map<std::string, std::string>& config) {
    std::string type;
    try {
        type = config.at("type");

    } catch ([[maybe_unused]] [[maybe_unused]] const std::out_of_range& e) {
        throw InvalidFormatException("Missing 'type' key in item configuration.");
    }

    std::string name;

    if (config.contains("name")) {
        name = config.at("name");

    } else {
        throw InvalidFormatException("Missing 'name' key for item type '" + type + "'.");
    }

    double unlockCost;
    if (config.contains("unlockCost")) {

        try {

            unlockCost = std::stod(config.at("unlockCost"));

        } catch ([[maybe_unused]] [[maybe_unused]] const std::invalid_argument& e) {
            throw InvalidFormatException("Invalid 'unlockCost' value for item '" + name + "'. Must be a number.");

        } catch ([[maybe_unused]] const std::out_of_range& e) {
            throw InvalidFormatException(" 'unlockCost' value out of range for item '" + name + "'.");
        }
    } else {
        throw InvalidFormatException("Missing 'unlockCost' key for item '" + name + "'.");
    }

    if (type == "Pastry") {
        double multiplier;
        if (config.contains("multiplier")) {

            try {
                multiplier = std::stod(config.at("multiplier"));

            } catch ([[maybe_unused]] const std::invalid_argument& e) {
                throw InvalidFormatException("Invalid 'multiplier' value for item '" + name + "'. Must be a number.");

            } catch ([[maybe_unused]] const std::out_of_range& e) {
                throw InvalidFormatException(" 'multiplier' value out of range for item '" + name + "'.");
            }
        } else {
            throw InvalidFormatException("Missing 'multiplier' key for item '" + name + "'.");
        }

        double baseIncome;

        if (config.contains("baseIncome")) {

            try {
                baseIncome = std::stod(config.at("baseIncome"));

            } catch ([[maybe_unused]] const std::invalid_argument& e) {
                throw InvalidFormatException("Invalid 'baseIncome' value for item '" + name + "'. Must be a number.");

            } catch ([[maybe_unused]] const std::out_of_range& e) {
                throw InvalidFormatException(" 'baseIncome' value out of range for item '" + name + "'.");
            }
        } else {
            throw InvalidFormatException("Missing 'baseIncome' key for item '" + name + "'.");
        }

        double upgradeCost;

        if (config.contains("upgradeCost")) {

            try {

                upgradeCost = std::stod(config.at("upgradeCost"));

            } catch ([[maybe_unused]] const std::invalid_argument& e) {
                throw InvalidFormatException("Invalid 'upgradeCost' value for item '" + name + "'. Must be a number.");

            } catch ([[maybe_unused]] const std::out_of_range& e) {
                throw InvalidFormatException(" 'upgradeCost' value out of range for item '" + name + "'.");
            }
        } else {
            throw InvalidFormatException("Missing 'upgradeCost' key for item '" + name + "'.");
        }

        double durationSec;

        if (config.contains("duration")) {

            try {
                durationSec = std::stod(config.at("duration"));

            } catch ([[maybe_unused]] const std::invalid_argument& e) {
                throw InvalidFormatException("Invalid 'duration' value for item '" + name + "'. Must be a number.");

            } catch ([[maybe_unused]] const std::out_of_range& e) {
                throw InvalidFormatException(" 'duration' value out of range for item '" + name + "'.");
            }
        } else {
            throw InvalidFormatException("Missing 'duration' key for item '" + name + "'.");
        }

        return std::make_unique<Pastry>(name, multiplier, unlockCost, baseIncome, upgradeCost, sf::seconds(static_cast<float>(durationSec)));

    } else if (type == "Beverage") {

        double multiplier;
        if (config.contains("multiplier")) {

            try {
                multiplier = std::stod(config.at("multiplier"));

            } catch ([[maybe_unused]] const std::invalid_argument& e) {
                throw InvalidFormatException("Invalid 'multiplier' value for item '" + name + "'. Must be a number.");

            } catch ([[maybe_unused]] const std::out_of_range& e) {
                throw InvalidFormatException(" 'multiplier' value out of range for item '" + name + "'.");
            }
        } else {
            throw InvalidFormatException("Missing 'multiplier' key for item '" + name + "'.");
        }
        std::vector<BeverageEffect> beverageEffects;

        if (config.contains("effects")) {
            std::istringstream ev(config.at("effects"));
            std::string eType;
            std::string eValue_str;
            while (ev >> eType >> eValue_str) {
                double eValue_numeric;

                try {
                    eValue_numeric = std::stod(eValue_str);

                } catch ([[maybe_unused]] const std::invalid_argument& e) {
                    throw InvalidFormatException("Invalid effect value '" + eValue_str + "' for item '" + name + "'. Must be a number.");

                } catch ([[maybe_unused]] const std::out_of_range& e) {
                    throw InvalidFormatException("Effect value '" + eValue_str + "' out of range for item '" + name + "'.");
                }
                beverageEffects.emplace_back(eType, eValue_numeric);
            }
        }
        std::string targetName = config.contains("target") ? config.at("target") : "ALL";
        return std::make_unique<Beverage>(name, multiplier, unlockCost, beverageEffects, targetName);

    } else if (type == "Sandwich") {
        double fastMultiplier;

        if (config.contains("fastMultiplier")) {

            try {
                fastMultiplier = std::stod(config.at("fastMultiplier"));

            } catch ([[maybe_unused]] const std::invalid_argument& e) {
                throw InvalidFormatException("Invalid 'fastMultiplier' value for item '" + name + "'. Must be a number.");

            } catch ([[maybe_unused]] const std::out_of_range& e) {
                throw InvalidFormatException(" 'fastMultiplier' value out of range for item '" + name + "'.");
            }
        } else {
            throw InvalidFormatException("Missing 'fastMultiplier' key for item '" + name + "'.");
        }

        double slowMultiplier;

        if (config.contains("slowMultiplier")) {

            try {
                slowMultiplier = std::stod(config.at("slowMultiplier"));

            } catch ([[maybe_unused]] const std::invalid_argument& e) {
                throw InvalidFormatException("Invalid 'slowMultiplier' value for item '" + name + "'. Must be a number.");

            } catch ([[maybe_unused]] const std::out_of_range& e) {
                throw InvalidFormatException(" 'slowMultiplier' value out of range for item '" + name + "'.");
            }
        } else {
            throw InvalidFormatException("Missing 'slowMultiplier' key for item '" + name + "'.");
        }

        double fastChance;
        if (config.contains("fastChance")) {

            try {
                fastChance = std::stod(config.at("fastChance"));

            } catch ([[maybe_unused]] const std::invalid_argument& e) {
                throw InvalidFormatException("Invalid 'fastChance' value for item '" + name + "'. Must be a number.");

            } catch ([[maybe_unused]] const std::out_of_range& e) {
                throw InvalidFormatException(" 'fastChance' value out of range for item '" + name + "'.");
            }
        } else {
            throw InvalidFormatException("Missing 'fastChance' key for item '" + name + "'.");
        }

        double durationSec;
        if (config.contains("duration")) {

            try {
                durationSec = std::stod(config.at("duration"));

            } catch ([[maybe_unused]] [[maybe_unused]] const std::invalid_argument& e) {
                throw InvalidFormatException("Invalid 'duration' value for item '" + name + "'. Must be a number.");

            } catch ([[maybe_unused]] const std::out_of_range& e) {
                throw InvalidFormatException(" 'duration' value out of range for item '" + name + "'.");
            }
        } else {
            throw InvalidFormatException("Missing 'duration' key for item '" + name + "'.");
        }
        return std::make_unique<Sandwich>(name, unlockCost, fastMultiplier, slowMultiplier, fastChance, sf::seconds(static_cast<float>(durationSec)));

    } else {
        throw InvalidFormatException("Unknown item type '" + type + "'");
    }
}

GameManager GameManager::loadFromFile(const std::string& fileName, Player& player) {
    std::ifstream file(fileName);
    if (!file.is_open())
        throw FileOpenException(fileName);

    std::vector<std::unique_ptr<Item>> items;
    std::vector<Delivery> deliveries;

    std::string line;
    std::map<std::string, std::string> currentItemConfig;
    std::string deliveryName;
    double unlockDeliveryCost = 0.0;

    auto processCurrentItem = [&]() {
        if (currentItemConfig.empty()) return;

        items.push_back(createItemFromConfig(currentItemConfig));
        deliveries.emplace_back(deliveryName, unlockDeliveryCost);

        currentItemConfig.clear();
        deliveryName.clear();
        unlockDeliveryCost = 0.0;
    };

    while (std::getline(file, line)) {
        if (line.empty()) {
            processCurrentItem();
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

        if (key == "deliveryName") {
            deliveryName = value;
        }
        else if (key == "unlockDeliveryCost") {

            try {
                unlockDeliveryCost = std::stod(value);

            } catch ([[maybe_unused]] const std::invalid_argument& e) {
                throw InvalidFormatException("Invalid 'unlockDeliveryCost' value in file '" + fileName + "'. Must be a number.");

            } catch ([[maybe_unused]] const std::out_of_range& e) {
                throw InvalidFormatException("'unlockDeliveryCost' value out of range in file '" + fileName + "'.");
            }
        } else {
            currentItemConfig[key] = value;
        }
    }
    processCurrentItem();

    std::cout << "Loaded " << items.size() << " items.\n";

    return { player, std::move(items), std::move(deliveries) };
}

void GameManager::saveGame() const {
    std::ofstream file("resources/savegame.txt");
    if (!file.is_open()) return;

    player.save(file);
    file << "itemsCount: " << items.size() << "\n\n";

    for (std::size_t i = 0; i < items.size(); ++i) {
        file << "item_idx: " << i << "\n";
        file << "unlocked: " << itemUnlocked[i] << "\n";
        file << "deliveryRunning: " << deliveryRunning[i] << "\n";
        items[i]->save(file);
        deliveries[i].save(file);
        file << "\n";
    }

    std::cout << "Game saved.\n";
}


bool GameManager::loadSavedGame() {
    std::ifstream file("resources/savegame.txt");
    if (!file.is_open())
        throw FileOpenException("resources/savegame.txt");

    player.load(file);

    std::string line;
    std::string key;
    std::string value;

    auto getKV = [&](std::string& k, std::string& v) {

        if (!std::getline(file, line)) return false;
        if (line.empty()) return false;

        const size_t pos = line.find(':');

        if (pos == std::string::npos) return false;

        k = line.substr(0, pos);
        v = line.substr(pos + 2);

        return true;
    };

    if (!getKV(key, value) || key != "itemsCount")
        throw SaveStateException("Missing itemsCount entry");

    std::size_t savedItemsCount;
    try {
        savedItemsCount = std::stoul(value);

    } catch ([[maybe_unused]] const std::invalid_argument &e) {
        throw SaveStateException("Invalid 'itemsCount' value in save file. Must be a number.");
    }
    catch ([[maybe_unused]] const std::out_of_range &e) {
        throw SaveStateException("'itemsCount' value out of range in save file.");
    }
    if (savedItemsCount != items.size()) {
        std::cerr << "Warning: Saved game has " << savedItemsCount
                << " items, but current game has " << items.size() << " items.\n";
    }

    for (std::size_t i = 0; i < savedItemsCount && i < items.size(); ++i) {
        if (!getKV(key, value) || key != "item_idx")
            throw SaveStateException("Missing item_idx entry for item " + std::to_string(i));


        if (!getKV(key, value) || key != "unlocked")
            throw SaveStateException("Missing unlocked entry for item " + std::to_string(i));

        try {
            itemUnlocked[i] = static_cast<bool>(std::stoi(value));

        } catch ([[maybe_unused]] const std::invalid_argument& e) {
            throw SaveStateException("Invalid 'unlocked' value for item " + std::to_string(i) + ". Must be 0 or 1.");
        }
        catch ([[maybe_unused]] const std::out_of_range& e) {
            throw SaveStateException("'unlocked' value out of range for item " + std::to_string(i) + ".");
        }

        if (!getKV(key, value) || key != "deliveryRunning")
            throw SaveStateException("Missing deliveryRunning entry for item " + std::to_string(i));

        try {
            deliveryRunning[i] = static_cast<bool>(std::stoi(value));
        }
        catch ([[maybe_unused]] const std::invalid_argument& e) {
            throw SaveStateException("Invalid 'deliveryRunning' value for item " + std::to_string(i) + ". Must be 0 or 1.");
        }
        catch ([[maybe_unused]] const std::out_of_range& e) {
            throw SaveStateException("'deliveryRunning' value out of range for item " + std::to_string(i) + ".");
        }

        items[i]->load(file);
        deliveries[i].load(file);

        if (deliveryRunning[i]) {
            runDeliveryLoop(*items[i], i);
        }
    }

    return true;
}


std::vector<std::unique_ptr<Item>>& GameManager::getItems() {
    return items;
}

std::vector<Delivery>& GameManager::getDelivery() {
    return deliveries;
}


double GameManager::getPlayerMoney() const {
    return player.getMoney();
}

void GameManager::useItem(const std::size_t index) {
    if (index >= items.size()) {
        throw InvalidIndexException("Attempted to use item at invalid index " + std::to_string(index) + ".");
    }
    if (!itemUnlocked[index]) {
        return;
    }

    const auto& item = items[index];

    if (dynamic_cast<Pastry*>(item.get())) {
        pushEventMessage(item->getName() + " cannot be actively used.");
        return;
    }

    const double cost = item->getUseCost();

    if (cost > 0 && !player.tryPay(cost)) {
        pushEventMessage("Not enough money to use " + item->getName());
        return;
    }

    if (const auto* sandwich = dynamic_cast<Sandwich*>(item.get())) {
        const double multiplier = sandwich->rollMultiplier();
        const sf::Time duration = sandwich->getDuration();

        const std::lock_guard lock(eventMutex);

        if (sandwichSpeedBuff.has_value()) {

            std::get<0>(*sandwichSpeedBuff) = multiplier;
            std::get<1>(*sandwichSpeedBuff) = duration;
            std::get<2>(*sandwichSpeedBuff) = duration;
            eventMessages.push("Refreshed " + item->getName() + "! Speed x" + std::to_string(multiplier) + " for " + std::to_string(duration.asSeconds()) + "s.");
        } else {

            sandwichSpeedBuff = std::make_tuple(multiplier, duration, duration);
            eventMessages.push("Used " + item->getName() + "! Speed x" + std::to_string(multiplier) + " for " + std::to_string(duration.asSeconds()) + "s.");
        }
        item->upgrade();
        return;
    }

    item->use(items, activeSpeedBuffs, eventMessages, eventMutex);
    item->upgrade();
}

void GameManager::update(const sf::Time time) {

    for (auto it = activeSpeedBuffs.begin(); it != activeSpeedBuffs.end(); ) {
        auto& remaining = std::get<2>(*it);
        remaining -= time;
        if (remaining <= sf::Time::Zero) {
            it = activeSpeedBuffs.erase(it);
        } else {
            ++it;
        }
    }

    if (sandwichSpeedBuff.has_value()) {
        auto& remaining = std::get<2>(*sandwichSpeedBuff);
        remaining -= time;
        if (remaining <= sf::Time::Zero) {
            sandwichSpeedBuff.reset(); // buff expired
        }
    }
}

double GameManager::combinedSpeedMultiplier() const {

    double combined = 1.0;
    for (const auto& buff : activeSpeedBuffs) {
        combined *= std::get<0>(buff);
    }

    if (sandwichSpeedBuff.has_value()) {
        combined *= std::get<0>(*sandwichSpeedBuff);
    }
    return combined;
}
