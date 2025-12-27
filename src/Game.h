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
#include <SFML/System/Clock.hpp>


class Game {
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
    mutable std::mutex progressMutex;

    enum class SellingState {
        Idle,
        Running
    };
    std::vector<SellingState> sellingState;
    std::vector<sf::Clock> sellingClock;
    std::vector<sf::Time> sellingDuration;


    std::thread runDeliveryLoop(Item& item, std::size_t index);

public:
    Game(Player& player_,
                std::vector<std::unique_ptr<Item>> items_,
                std::vector<Delivery> deliveries_);
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    ~Game();
    friend std::ostream& operator<<(std::ostream& ostream, const Game& manager);

    static Game loadFromFile(const std::string& fileName, Player& player);
    void saveGame() const;
    bool loadSavedGame();
    void resetFromFile(const std::string& file);


    void pushEventMessage(const std::string& message);

    std::string unlockItem(std::size_t index);
    [[nodiscard]] bool isUnlocked(std::size_t index) const;
    [[nodiscard]] bool isSelling(std::size_t index) const;


    void sell(const Item& item) const;
    void runSellingLoop(const Item& item, std::size_t index);

    void updateSelling();

    void upgrade(Item& item) const;
    [[nodiscard]] float anyProgress(std::size_t index) const;

    // void startDelivery(Item& item, const Delivery& delivery, int index);
    void stopAllDeliveries();

    void useItem(std::size_t index);
    void update(sf::Time time) const;
    [[nodiscard]] double combinedSpeedMultiplier() const;
    [[nodiscard]] double computeTotalIncomePerSecond() const;

    std::vector<std::unique_ptr<Item>>& getItems();
    // std::vector<Delivery>& getDelivery();
    [[nodiscard]] double getPlayerMoney() const;
};

#endif // OOP_GAMEMANAGER_H
