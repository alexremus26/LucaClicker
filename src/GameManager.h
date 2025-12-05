#ifndef OOP_GAMEMANAGER_H
#define OOP_GAMEMANAGER_H

#include <vector>
#include <memory> // REQUIRED for std::unique_ptr
#include <string>
#include "Player.h"
#include "Item.h"
#include "Delivery.h"
#include <SFML/Graphics.hpp>

class Item;
class Player; // Likely needed too based on context

class GameManager {
private:
    Player& player;
    std::vector<std::unique_ptr<Item>> items;
    std::vector<Delivery> deliveries;
    std::vector<bool> deliveryRunning;

    void runDeliveryLoop(Item& item, Delivery& delivery, int index);

public:
    // CHANGED: Constructor accepts unique_ptrs
    GameManager(Player& player_, std::vector<std::unique_ptr<Item>> items_, std::vector<Delivery> deliveries_);

    GameManager(const GameManager& other);
    ~GameManager();
    GameManager& operator=(const GameManager& other);
    friend std::ostream& operator<<(std::ostream& ostream, const GameManager& manager);

    static GameManager loadFromFile(const std::string& fileName, Player& player);
    void sell(const Item& item) const;
    void upgrade(Item& item) const;
    void startDelivery(Item& item, Delivery& delivery, int index);
    void stopAllDeliveries();

    // CHANGED: Returns reference to vector of pointers
    std::vector<std::unique_ptr<Item>>& getItems();
    std::vector<Delivery>& getDelivery();

    void saveGame() const;
    bool loadSavedGame();
};

#endif //OOP_GAMEMANAGER_H