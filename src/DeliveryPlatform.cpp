#include "DeliveryPlatform.h"
#include <iostream>

double DeliveryPlatform::computeIncome(const Item& item) const {
    return doComputeIncome(item);
}

sf::Time DeliveryPlatform::computeSpeed(const Item& item) const {
    return doComputeSpeed(item);
}

DeliveryPlatform::~DeliveryPlatform() { std::cout << "DeliveryPlatform was destroyed!";}
