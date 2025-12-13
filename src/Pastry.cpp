#include "Pastry.h"
#include <iostream>

Pastry::Pastry(std::string name_, const double multiplier_, const double unlockCost_,
               const double baseIncome_, const double upgradeCost_, const sf::Time duration_)
    : Item(std::move(name_), multiplier_, unlockCost_),
      baseIncome(baseIncome_),
      upgradeCost(upgradeCost_),
      duration(duration_) {
}

Pastry::Pastry(const Pastry &other)
    : Item(other),
      baseIncome(other.baseIncome),
      upgradeCost(other.upgradeCost),
      duration(other.duration) {
}

Pastry::~Pastry() {
    std::cout << "Pastry " << getName() << " a fost distrus!\n";
}

Pastry &Pastry::operator=(const Pastry &other) {
    if (this != &other) {
        Item::operator=(other);
        baseIncome = other.baseIncome;
        upgradeCost = other.upgradeCost;
        duration = other.duration;
    }
    return *this;
}

Item *Pastry::clone() const {
    return new Pastry(*this);
}

void Pastry::doPrint(std::ostream &os) const {
    os << "Pastry: " << name
            << " | Income: " << baseIncome
            << " | Upgrade Cost: " << upgradeCost
            << " | Level: " << level
            << " | Multiplier: " << multiplier;
}


void Pastry::doApplyMultiplier(const double mult) {
    multiplier *= mult;
}

void Pastry::doUpgrade() {
    level++;
    baseIncome *= 1.15;
    upgradeCost *= 1.5;
}


sf::Time Pastry::doGetDuration() const {
    return sf::seconds(2.0f + static_cast<float>(getUnlockCost()) / 100.0f);
}

double Pastry::doGetBaseIncome() const {
    return baseIncome;
}

double Pastry::doGetUpgradeCost() const {
    return upgradeCost;
}

std::string Pastry::doGetEffectDescription() const {
    return "Generates " + std::to_string(static_cast<int>(getBaseIncome())) + " RON";
}

void Pastry::doSetBaseIncome(const double newBaseIncome) { baseIncome = newBaseIncome; }
void Pastry::doSetUpgradeCost(const double newUpgradeCost) { upgradeCost = newUpgradeCost; }

