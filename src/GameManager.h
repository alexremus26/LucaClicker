#ifndef OOP_GAMEMANAGER_H
#define OOP_GAMEMANAGER_H

#include <vector>
#include <memory>
#include <string>
#include <ranges>
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

    // Unlock system
    int unlockItem(std::size_t index);
    bool isUnlocked(std::size_t index) const;
    const std::vector<bool>& getUnlocked() const;

    // Item logic
    void sell(const Item& item) const;
    void upgrade(Item& item) const;

    // Delivery logic
    void startDelivery(Item& item, const Delivery& delivery, int index);
    static void stopAllDeliveries();

    // Beverage effects
    void applyAllBeverageEffects() const;
    static void applyBeverageToItem(const Beverage& bev, Item& target);

    // Accessors
    std::vector<std::unique_ptr<Item>>& getItems();
    std::vector<Delivery>& getDelivery();

    // Save/load
    void saveGame() const;
    bool loadSavedGame();
};

#endif
