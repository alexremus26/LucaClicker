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


Item *Pastry::clone() const {
    return new Pastry(*this);
}


double Pastry::doSellPayout() const {
    return baseIncome;
}

double Pastry::doDeliveryPayout() const {
    return baseIncome;
}

void Pastry::doPrint(std::ostream &os) const {
        os  << "  Income: " << baseIncome
            << " | Upgrade Cost: " << upgradeCost
            << " | Level: " << level
            << " | Multiplier: " << multiplier;
}


void Pastry::doApplyMultiplier(const double mult) {
    multiplier *= mult;
}

void Pastry::doUpgrade() {
    {
        level++;
        baseIncome *= 1.15;
        upgradeCost *= 1.5;
    }
}


sf::Time Pastry::doComputeDuration() const {
    return sf::seconds(2.0f + static_cast<float>(unlockCost) / 100.0f);
}

double Pastry::getBaseIncome() const {
    return baseIncome;
}

double Pastry::getUpgradeCost() const {
    return upgradeCost;
}

std::string Pastry::doGetEffectDescription() const {
    return "Generates " + std::to_string(static_cast<int>(getBaseIncome())) + " RON";
}

void Pastry::doSetBaseIncome(const double newBaseIncome) { baseIncome = newBaseIncome; }
void Pastry::doSetUpgradeCost(const double newUpgradeCost) { upgradeCost = newUpgradeCost; }

