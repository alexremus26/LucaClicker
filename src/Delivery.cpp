#include "Delivery.h"
#include "GlovoPlatform.h"
#include "WoltPlatform.h"
#include <iostream>
#include <utility>

Delivery::Delivery(std::string name_, const double unlockCost_)
    : name(std::move(name_)),
      unlockDeliveryCost(unlockCost_),
      platform(nullptr)
{
    std::cout << "Delivery \"" << name << "\" created!\n";

    if (name == "Glovo") {
        platform = std::make_unique<GlovoPlatform>();
    }
    else if (name == "Wolt") {
        platform = std::make_unique<WoltPlatform>();
    }
    else {
        std::cout << "Warning: Unknown platform \"" << name
                  << "\". Defaulting to Glovo.\n";
        platform = std::make_unique<GlovoPlatform>();
    }
}

Delivery::Delivery(const Delivery& other)
    : name(other.name),
      unlockDeliveryCost(other.unlockDeliveryCost),
      running(other.running),
      platform(other.platform ? other.platform->clone() : nullptr)
{
    std::cout << "Delivery \"" << name << "\" copied!\n";
}

Delivery& Delivery::operator=(Delivery other) {
    swap(*this, other);
    return *this;
}

Delivery::~Delivery() {
    std::cout << "Delivery \"" << name << "\" destroyed!\n";
}

void swap(Delivery& a, Delivery& b) noexcept {
    using std::swap;
    swap(a.name, b.name);
    swap(a.unlockDeliveryCost, b.unlockDeliveryCost);
    swap(a.running, b.running);
    swap(a.platform, b.platform);
}

const std::string& Delivery::getName() const {
    return name;
}

double Delivery::getUnlockCost() const {
    return unlockDeliveryCost;
}

DeliveryPlatform& Delivery::getPlatform() const {
    return *platform;
}


std::ostream& operator<<(std::ostream& os, const Delivery& d) {
    os << "Delivery [" << d.name
       << " | unlock: " << d.unlockDeliveryCost
       << " | running: " << (d.running ? "YES" : "NO")
       << "]";
    return os;
}
