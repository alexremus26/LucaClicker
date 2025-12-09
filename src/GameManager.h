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

    void runDeliveryLoop(Item& item, std::size_t index);

public:
    GameManager(Player& player_, std::vector<std::unique_ptr<Item>> items_, std::vector<Delivery> deliveries_);
    GameManager(const GameManager& other);
    ~GameManager();
    GameManager& operator=(const GameManager& other);
    friend std::ostream& operator<<(std::ostream& ostream, const GameManager& manager);

    static GameManager loadFromFile(const std::string& fileName, Player& player);

    void sell(const Item& item) const;
    void upgrade(Item& item) const;

    void startDelivery(Item& item, const Delivery& delivery, int index);
    void stopAllDeliveries();

    void applyAllBeverageEffects() const;
    static void applyBeverageToItem(const Beverage& bev, Item& target) ;

    std::vector<std::unique_ptr<Item>>& getItems();
    std::vector<Delivery>& getDelivery();

    void saveGame() const;
    bool loadSavedGame();
};

#endif //OOP_GAMEMANAGER_H
