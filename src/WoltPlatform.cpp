#include "WoltPlatform.h"
#include <iostream>

WoltPlatform::WoltPlatform() : DeliveryPlatform(0.05, 7.0) {
}

WoltPlatform::~WoltPlatform() = default;

IncomeResult WoltPlatform::doComputeIncome(const Item& item) const {
    double income = item.deliveryPayout() * 1.4;
    std::string message;
    if (rollCrit()) {
        message = "MEGA CRITICAL DELIVERY!";
        income *= critMultiplier;
    }
    return {income, message};
}

sf::Time WoltPlatform::doComputeSpeed(const Item& item) const {
    const double base = item.deliveryPayout();
    const double seconds = MIN_DURATION + base * DURATION_SCALE;
    return sf::seconds(static_cast<float>(seconds));
}

WoltPlatform* WoltPlatform::clone() const {
    return new WoltPlatform(*this);
}
