#include "GlovoPlatform.h"
#include <iostream>

GlovoPlatform::GlovoPlatform() : DeliveryPlatform(0.10, 3.0) {
}

GlovoPlatform::~GlovoPlatform() = default;

IncomeResult GlovoPlatform::doComputeIncome(const Item& item) const {
    double income = item.deliveryPayout();
    std::string message;
    if (rollCrit()) {
        message = "CRITICAL DELIVERY!";
        income *= critMultiplier;
    }
    return {income, message};
}

sf::Time GlovoPlatform::doComputeSpeed(const Item& item) const {
    const double seconds = MIN_DURATION + item.deliveryPayout() * DURATION_SCALE;
    return sf::seconds(static_cast<float>(seconds));
}

GlovoPlatform* GlovoPlatform::clone() const {
    return new GlovoPlatform(*this);
}
