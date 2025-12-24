#include "Game.h"
#include "GameExceptions.h"
#include "ItemFactory.h"
#include "Beverage.h"
#include "Pastry.h"
#include "Sandwich.h"
#include "RaffleTicket.h"

#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>
#include <map>
#include <tuple>
#include <SFML/System/Clock.hpp>
#include <algorithm>


Game::Game(Player& player_,
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



Game::~Game() {
    stopAllDeliveries();
    std::cout << "GameManager destroyed!\n";
}

std::ostream& operator<<(std::ostream& ostream, const Game& manager)
{
    ostream << "=== Game Manager ===\n";
    ostream << "Player money: " << manager.player.getMoney() << "\n";
    ostream << "Items:\n";

    for (const auto& item : manager.items)
        ostream << "  " << *item << "\n";

    return ostream;
}

std::string Game::unlockItem(const std::size_t index) {
    if (index >= itemUnlocked.size()) {
        throw InvalidIndexException("Attempted to unlock item at invalid index " + std::to_string(index) + ".");
    }
    if (itemUnlocked[index])
        return "Item already unlocked!";

    const double cost = items[index]->getUnlockCost();
    if (!player.tryPay(cost))
        return "Not enough money!";

    itemUnlocked[index] = true;

    return "Unlocked item!";
}

bool Game::isUnlocked(const std::size_t index) const {
    return index < itemUnlocked.size() && itemUnlocked[index];
}


std::thread Game::runDeliveryLoop(Item& item, std::size_t index) {
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
void Game::runSellingLoop(Item& item, std::size_t index)
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


void Game::sell(const Item& item) const {
    player.earn(item.sellPayout());
}

void Game::upgrade(Item& item) const{
    const double cost = item.getUpgradeCost();
    if (cost > 0 && player.tryPay(cost)) {
        item.upgrade();
    }
}


float Game::anyProgress(const std::size_t index) const {
    return (index < progress.size()) ? progress[index] : 0.f;
}

void Game::startDelivery(Item& item, const Delivery& delivery, const int index) {
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

void Game::stopAllDeliveries() {
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

void Game::pushEventMessage(const std::string& message) {
    const std::lock_guard<std::mutex> lock(eventMutex);
    eventMessages.push(message);
}

std::string Game::popEventMessage() {
    const std::lock_guard<std::mutex> lock(eventMutex);
    if (eventMessages.empty()) {
        return "";
    }
    std::string message = eventMessages.front();
    eventMessages.pop();
    return message;
}

double Game::computeTotalIncomePerSecond() const {
    double totalIncome = 0.0;
    for (size_t i = 0; i < items.size(); ++i) {
        if (isUnlocked(i) && deliveryRunning.at(i)) {
            totalIncome += items[i]->computeIncomePerSecond();
        }
    }
    return totalIncome;
}


Game Game::loadFromFile(const std::string& fileName, Player& player) {
    Beverage::registerItem();
    Pastry::registerItem();
    Sandwich::registerItem();
    RaffleTicket::registerItem();

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

        const auto typeIt = currentItemConfig.find("type");
        if (typeIt == currentItemConfig.end()) {
            throw InvalidFormatException("Missing 'type' for an item in file '" + fileName + "'.");
        }

        auto created = ItemFactory::getInstance().create(typeIt->second, currentItemConfig);
        if (!created) {
            throw InvalidFormatException("Unknown item type '" + typeIt->second + "' in file '" + fileName + "'.");
        }

        items.push_back(std::move(created));
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

void Game::saveGame() const {
    std::ofstream file("../data/savefile.txt");
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


bool Game::loadSavedGame() {
    std::ifstream file("../data/savefile.txt");
    if (!file.is_open())
        throw FileOpenException("data/savefile.txt");

    player.load(file);

    std::string line;
    std::string key;
    std::string value;

    auto getKV = [&](std::string& k, std::string& v) {

        while (std::getline(file, line)) {
            if (line.empty()) continue;
            break;
        }

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
            deliveryThreads.emplace_back(runDeliveryLoop(*items[i], i));
        }
    }

    return true;
}

std::vector<std::unique_ptr<Item>>& Game::getItems() {
    return items;
}

std::vector<Delivery>& Game::getDelivery() {
    return deliveries;
}


double Game::getPlayerMoney() const {
    return player.getMoney();
}

void Game::useItem(const std::size_t index) {
    if (index >= items.size()) {
        throw InvalidIndexException("Attempted to use item at invalid index " + std::to_string(index) + ".");
    }
    if (!itemUnlocked[index]) {
        return;
    }

    const auto& item = items[index];

    if (!item->isUsable()) {
        pushEventMessage(item->getName() + " cannot be actively used.");
        return;
    }

    const double cost = item->getUseCost();

    if (cost > 0 && !player.tryPay(cost)) {
        pushEventMessage("Not enough money to use " + item->getName());
        return;
    }

    item->use(items, eventMessages, eventMutex);

    const double totalIncome = computeTotalIncomePerSecond();

    item->drawRaffle(player, totalIncome, eventMessages, eventMutex);
    item->upgrade();
}

void Game::update(const sf::Time time) const {
    for (const auto& item_ptr : items) {
        item_ptr->update(time);
    }
}

double Game::combinedSpeedMultiplier() const {
    double combined = 1.0;
    for (const auto& item_ptr : items) {
        combined *= item_ptr->getSpeedMultiplier();
    }
    return combined;
}
