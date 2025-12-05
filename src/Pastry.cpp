#include "Pastry.h"
#include <iostream>

Pastry::Pastry(std::string name_, double baseIncome_, double upgradeCost_,
               double unlockCost_, double multiplier_, sf::Time duration_)
    : Item(std::move(name_), baseIncome_, upgradeCost_,  multiplier_, unlockCost_,duration_) {}

Pastry::Pastry(const Pastry& other)
    : Item(other) {}

Pastry::~Pastry() {
    std::cout << "Pastry " << getName() << " a fost distrus!\n";
}

Pastry& Pastry::operator=(const Pastry& other) {
    if (this != &other) {
        Item::operator=(other);
    }
    return *this;
}

Item* Pastry::clone() const {
    return new Pastry(*this);
}

void Pastry::doPrint(std::ostream &os) const {
    os << "Pastry: " << name
       << " | Income: " << baseIncome
       << " | Upgrade Cost: " << upgradeCost
       << " | Level: " << level
       << " | Multiplier: " << multiplier;
}

void Pastry::doApplyEffect() const {
    // Pastries don't apply effects to other items
}

void Pastry::doApplyMultiplier(double mult) {
    multiplier *= mult;
}

void Pastry::doUpgrade() {
    level++;
    baseIncome *= 1.15;
    upgradeCost *= 1.5;
}

double Pastry::doCalculateRevenue() const {
    return baseIncome * level * multiplier;
}

sf::Time Pastry::doGetDuration() const {
    return sf::seconds(2.0f + static_cast<float>(getUnlockCost()) / 100.0f);
}

std::string Pastry::doGetEffectDescription() const {
    return "Generates " + std::to_string(static_cast<int>(doCalculateRevenue())) + " RON";
}