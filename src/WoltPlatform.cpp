#include "WoltPlatform.h"

WoltPlatform::WoltPlatform() {
    std::cout << "WoltPlatform created!\n";
}

WoltPlatform::~WoltPlatform() {
    std::cout << "WoltPlatform was destroyed!\n";
}

double WoltPlatform::doComputeIncome(const Item& item) const {
    return item.getBaseIncome() * 1.4;   // +40% income
}

sf::Time WoltPlatform::doComputeSpeed(const Item& item) const {
    const double base = item.getBaseIncome();
    const double seconds = MIN_DURATION + base * DURATION_SCALE;
    return sf::seconds(static_cast<float>(seconds));
}

WoltPlatform* WoltPlatform::clone() const {
    return new WoltPlatform(*this);
}
