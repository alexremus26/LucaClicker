#ifndef OOP_GAMEMANAGER_H
#define OOP_GAMEMANAGER_H

#include <vector>
#include <memory>
#include <string>
#include "Player.h"
#include "Item.h"
#include "Beverage.h"
#include "Delivery.h"
#include <SFML/Graphics.hpp>

class GameManager {
private:
    Player& player;
    std::vector<std::unique_ptr<Item>> items;
    std::vector<Delivery> deliveries;

    std::vector<bool> deliveryRunning;
    std::vector<bool> itemUnlocked;
    std::vector<float> sellProgress;
    std::vector<bool> sellingRunning;


    void runDeliveryLoop(Item& item, std::size_t index);

public:
    GameManager(Player& player_,
                std::vector<std::unique_ptr<Item>> items_,
                std::vector<Delivery> deliveries_);

    GameManager(const GameManager& other);
    GameManager& operator=(const GameManager& other);
    ~GameManager();

    friend std::ostream& operator<<(std::ostream& ostream, const GameManager& manager);

    static GameManager loadFromFile(const std::string& fileName, Player& player);

    int unlockItem(std::size_t index);
    [[nodiscard]] bool isUnlocked(std::size_t index) const;
    [[nodiscard]] const std::vector<bool>& getUnlocked() const;

    void sell(const Item& item) const;
    void runSellingLoop (Item& item, std::size_t index);
    void upgrade(Item& item) const;
    [[nodiscard]] float getSellProgress(std::size_t index) const;
    bool isSelling(std::size_t index) const;

    void startDelivery(Item& item, const Delivery& delivery, int index);
    static void stopAllDeliveries();

    void applyAllBeverageEffects() const;
    static void applyBeverageToItem(const Beverage& bev, Item& target);

    std::vector<std::unique_ptr<Item>>& getItems();
    std::vector<Delivery>& getDelivery();

    void saveGame() const;
    bool loadSavedGame();
};

#endif
