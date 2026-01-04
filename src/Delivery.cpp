#include "Delivery.h"
#include "GlovoPlatform.h"
#include "WoltPlatform.h"
#include "GameExceptions.h"
#include <iostream>
#include <utility>

Delivery::Delivery(std::string name_, const double unlockCost_)
    : name(std::move(name_)),
      unlockDeliveryCost(unlockCost_),
      platform(nullptr)
{
    if (name == "Glovo") {
        platform = std::make_unique<GlovoPlatform>();
    }
    else if (name == "Wolt") {
        platform = std::make_unique<WoltPlatform>();
    }
    else {
        platform = std::make_unique<GlovoPlatform>();
    }
}

Delivery::Delivery(const Delivery& other)
    : name(other.name),
      unlockDeliveryCost(other.unlockDeliveryCost),
      running(other.running),
      platform(other.platform ? other.platform->clone() : nullptr)
{
}

Delivery& Delivery::operator=(Delivery other) {
    swap(*this, other);
    return *this;
}

Delivery::~Delivery() = default;

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

void Delivery::save(std::ostream& os) const {
    os << "deliveryName: " << name << "\n";
    os << "unlockDeliveryCost: " << unlockDeliveryCost << "\n";
    os << "running: " << running << "\n";
}

void Delivery::load(std::istream& is) {
    std::string line;
    std::string key;
    std::string value;

    auto getKV = [&](std::string& k, std::string& v) {
        if (!std::getline(is, line)) return false;
        if (line.empty()) return false;
        const size_t pos = line.find(':');
        if (pos == std::string::npos) return false;
        k = line.substr(0, pos);
        v = line.substr(pos + 2);
        return true;
    };

    while (getKV(key, value)) {
        if (key == "deliveryName") {
            name = value;

            if (name == "Wolt") {
                platform = std::make_unique<WoltPlatform>();
            } else {
                platform = std::make_unique<GlovoPlatform>();
            }
        }
        else if (key == "unlockDeliveryCost") unlockDeliveryCost = std::stod(value);
        else if (key == "running") running = std::stoi(value);
    }
}


std::ostream& operator<<(std::ostream& os, const Delivery& d) {
    os << "Delivery [" << d.name
       << " | unlock: " << d.unlockDeliveryCost
       << " | running: " << (d.running ? "YES" : "NO")
       << "]";
    return os;
}
