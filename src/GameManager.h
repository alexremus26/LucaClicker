#ifndef OOP_GAMEMANAGER_H
#define OOP_GAMEMANAGER_H

#include <vector>
#include "Player.h"
#include "FoodItem.h"
#include "Delivery.h"

class GameManager {
    Player& player;
    std::vector<FoodItem> foodItems;
    std::vector<Delivery> deliveries;
    bool deliveryRunning = false;

    void runDeliveryLoop(FoodItem& food, Delivery& delivery) const;

public:
    GameManager(Player& player_, std::vector<FoodItem> foodItem_, std::vector<Delivery> deliveries_);
    GameManager(const GameManager& gameManager);
    ~GameManager();
    GameManager& operator=(const GameManager& manager);
    friend std::ostream& operator<<(std::ostream& ostream, const GameManager& manager);

    static GameManager loadFromFile(const std::string& fileName, Player& player);
    void sell(const FoodItem& foodItem) const;
    void upgrade(FoodItem& foodItem) const;
    void startDelivery(FoodItem& foodItem, Delivery& delivery);
    void stopDelivery();
    std::vector<FoodItem>& getFoods();
    std::vector<Delivery>& getDelivery();
};


#endif //OOP_GAMEMANAGER_H