#ifndef OOP_GAMEMANAGER_H
#define OOP_GAMEMANAGER_H

#include <vector>
#include "Player.h"
#include "FoodItem.h"
#include "Delivery.h"
#include <SFML/Graphics.hpp>

class GameManager {
    Player& player;
    std::vector<FoodItem> foodItems;
    std::vector<Delivery> deliveries;
    std::vector<bool> deliveryRunning;

    void runDeliveryLoop(FoodItem& food, Delivery& delivery, int index);

public:
    GameManager(Player& player_, std::vector<FoodItem> foodItem_, std::vector<Delivery> deliveries_);
    GameManager(const GameManager& gameManager);
    ~GameManager();
    GameManager& operator=(const GameManager& manager);
    friend std::ostream& operator<<(std::ostream& ostream, const GameManager& manager);

    static GameManager loadFromFile(const std::string& fileName, Player& player);
    void sell(const FoodItem& foodItem) const;
    void upgrade(FoodItem& foodItem) const;
    void startDelivery(FoodItem& foodItem, Delivery& delivery, int index);
    void stopAllDeliveries();
    std::vector<FoodItem>& getFoods();
    std::vector<Delivery>& getDelivery();
    void saveGame() const;
    bool loadSavedGame();
};


#endif //OOP_GAMEMANAGER_H