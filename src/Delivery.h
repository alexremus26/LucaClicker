#ifndef OOP_DELIVERY_H
#define OOP_DELIVERY_H

#include <memory>
#include <string>
#include <SFML/System/Time.hpp>
#include "DeliveryPlatform.h"

class Delivery {
private:
    std::string name;
    double unlockDeliveryCost;
    bool running = false;

    std::unique_ptr<DeliveryPlatform> platform;

public:
    Delivery(std::string name_, double unlockCost_);
    Delivery(const Delivery& other);
    Delivery& operator=(Delivery other);
    ~Delivery();
    friend void swap(Delivery& a, Delivery& b) noexcept;
    friend std::ostream& operator<<(std::ostream& os, const Delivery& d);

    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] double getUnlockCost() const;
    [[nodiscard]] DeliveryPlatform& getPlatform() const;

    void setRunning(bool r);
    [[nodiscard]] bool isRunning() const;
};

#endif // OOP_DELIVERY_H
