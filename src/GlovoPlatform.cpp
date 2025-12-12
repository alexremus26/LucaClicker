#include "GlovoPlatform.h"
#include <iostream>

GlovoPlatform::GlovoPlatform() {
    std::cout << "GlovoPlatform created!\n";
}

GlovoPlatform::~GlovoPlatform() {
    std::cout << "GlovoPlatform destroyed!\n";
}

double GlovoPlatform::doComputeIncome(const Item& item) const {
    // Glovo gives  income bonus
    return item.getBaseIncome();
}

sf::Time GlovoPlatform::doComputeSpeed(const Item& item) const {
    const double seconds = MIN_DURATION + item.getBaseIncome() * DURATION_SCALE;
    return sf::seconds(static_cast<float>(seconds));
}

GlovoPlatform* GlovoPlatform::clone() const {
    return new GlovoPlatform(*this);
}
