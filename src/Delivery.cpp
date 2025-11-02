#include "Delivery.h"
#include <iostream>

Delivery::Delivery(std::string name, const double &unlockDeliveryCost_)
    : deliveryName(std::move(name)), unlockDeliveryCost(unlockDeliveryCost_) {
}

Delivery::Delivery(const Delivery &delivery)
    : deliveryName(delivery.deliveryName),
      unlockDeliveryCost(delivery.unlockDeliveryCost),
      timeInterval(delivery.timeInterval),
      running(delivery.running) {
}

Delivery::~Delivery() { std::cout << "Curierul " << deliveryName << " a fost distrus! \n"; }

Delivery &Delivery::operator=(const Delivery &delivery) {
    deliveryName = delivery.deliveryName;
    timeInterval = delivery.timeInterval;
    unlockDeliveryCost = delivery.unlockDeliveryCost;
    return *this;
}

std::ostream &operator<<(std::ostream &ostream, const Delivery &delivery) {
    ostream << "Delivery:" << delivery.deliveryName << "Unlock Cost:" << delivery.unlockDeliveryCost << "  SaleRate:" <<
            delivery.timeInterval.asSeconds() << std::endl;
    return ostream;
}

const double &Delivery::getUnlockCost() const {
    return unlockDeliveryCost;
}

sf::Time Delivery::getTimeInterval() const {
    return timeInterval;
}

bool Delivery::canUnlock(const Player &player) const {
    return player.getMoney() >= unlockDeliveryCost;
}





