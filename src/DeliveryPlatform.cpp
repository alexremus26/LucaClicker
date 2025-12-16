#include "DeliveryPlatform.h"
#include <iostream>

DeliveryPlatform::DeliveryPlatform(const double critChance, const double critMultiplier)
    : critChance(critChance), critMultiplier(critMultiplier), rng(std::random_device{}()) {}


IncomeResult DeliveryPlatform::computeIncome(const Item& item) const {
    return doComputeIncome(item);
}

sf::Time DeliveryPlatform::computeSpeed(const Item& item) const {
    return doComputeSpeed(item);
}

bool DeliveryPlatform::rollCrit() const {
    std::uniform_real_distribution dist(0.0, 1.0);
    return dist(rng) < critChance;
}

DeliveryPlatform::~DeliveryPlatform() { std::cout << "DeliveryPlatform was destroyed!";}
