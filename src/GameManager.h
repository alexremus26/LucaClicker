#ifndef OOP_GAMEMANAGER_H
#define OOP_GAMEMANAGER_H

#include <vector>
#include <memory>
#include <string>
#include <thread>
#include <queue>
#include <mutex>
#include <SFML/System/Time.hpp>

#include "Player.h"
#include "Item.h"
#include "Delivery.h"

class GameManager {
private:
    Player& player;
    std::vector<std::unique_ptr<Item>> items;
    std::vector<Delivery> deliveries;

    std::vector<bool> deliveryRunning;
    std::vector<bool> itemUnlocked;
    std::vector<float> progress;
    std::vector<bool> sellingRunning;

    std::queue<std::string> eventMessages;
    std::mutex eventMutex;
    std::vector<std::thread> deliveryThreads;

    std::thread runDeliveryLoop(Item& item, std::size_t index);

public:
    GameManager(Player& player_,
                std::vector<std::unique_ptr<Item>> items_,
                std::vector<Delivery> deliveries_);
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;
    ~GameManager();

    friend std::ostream& operator<<(std::ostream& ostream, const GameManager& manager);

    static GameManager loadFromFile(const std::string& fileName, Player& player);
    void saveGame() const;
    bool loadSavedGame();

    void pushEventMessage(const std::string& message);
    std::string popEventMessage();

    std::string unlockItem(std::size_t index);
    [[nodiscard]] bool isUnlocked(std::size_t index) const;

    void sell(const Item& item) const;
    void runSellingLoop(Item& item, std::size_t index);
    void upgrade(Item& item) const;
    [[nodiscard]] float getProgress(std::size_t index) const;

    void startDelivery(Item& item, const Delivery& delivery, int index);
    void stopAllDeliveries();

    void useItem(std::size_t index);
    void update(sf::Time time);
    [[nodiscard]] double combinedSpeedMultiplier() const;

    std::vector<std::unique_ptr<Item>>& getItems();
    std::vector<Delivery>& getDelivery();
    [[nodiscard]] double getPlayerMoney() const;
};

#endif // OOP_GAMEMANAGER_H
