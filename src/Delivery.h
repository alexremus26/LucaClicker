#ifndef OOP_DELIVERY_H
#define OOP_DELIVERY_H

#include <SFML/System/Time.hpp>
#include "Player.h"

class Delivery {
    std::string deliveryName;
    double unlockDeliveryCost;
    sf::Time timeInterval = sf::seconds(2.0f);
    bool running = false;

public:
    Delivery(std::string  name, const double& unlockDeliveryCost_);
    Delivery(const Delivery& delivery);
    ~Delivery();
    Delivery& operator=(const Delivery& delivery);
    friend std::ostream& operator<<(std::ostream& ostream, const Delivery& delivery);

    bool canUnlock(const Player& player) const;
    sf::Time getTimeInterval() const;
    const double& getUnlockCost() const;
};


#endif //OOP_DELIVERY_H