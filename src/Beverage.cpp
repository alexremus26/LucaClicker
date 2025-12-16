#include "Beverage.h"
#include "Pastry.h"
#include <iostream>
#include <sstream>

Beverage::Beverage(std::string name_, const double multiplier_, const double unlockCost_,
                   std::vector<BeverageEffect> effects_,
                   std::string targetName_)
    : Item(std::move(name_), multiplier_, unlockCost_),
      effects(std::move(effects_)),
      targetName(std::move(targetName_)) {}

Beverage::Beverage(const Beverage& other)
    : Item(other),
      effects(other.effects),
      targetName(other.targetName) {}

Beverage::~Beverage() {
    std::cout << "Beverage " << name << " destroyed\n";
}


Item* Beverage::clone() const {
    return new Beverage(*this);
}

const std::vector<BeverageEffect>& Beverage::getEffects() const {
    return effects;
}

double Beverage::doSellPayout() const {
    return 0.0;
}

double Beverage::doDeliveryPayout() const {
    return 0.0;
}

void Beverage::doPrint(std::ostream& os) const {
    os << "Beverage: " << name
       << " | Multiplier: " << multiplier
       << " | Unlock cost: " << unlockCost
       << " | Level: " << level
       << " | Target: " << targetName
       << " | Effects: " << getEffectDescription();
}

void Beverage::doApplyMultiplier(const double mult) {
    multiplier *= mult;
}

void Beverage::doUpgrade() {
    level++;
    multiplier *= 1.1;
}

sf::Time Beverage::doComputeDuration() const {
    return sf::seconds(0.f);
}

std::string Beverage::doGetEffectDescription() const {
    std::ostringstream os;
    bool first = true;

    for (const auto& effect : effects) {
        const auto& [type, value] = effect;

        if (!first) os << ", ";
        first = false;

        if (type == "profit_multiplier")
            os << "Profit x" << value;
        else if (type == "upgrade_discount")
            os << "Upgrade cost x" << value;
        else
            os << type << "(" << value << ")";
    }

    if (first) os << "No effects";
    return os.str();
}

void Beverage::doSetBaseIncome(double) {}
void Beverage::doSetUpgradeCost(double) {}

void Beverage::setEffects(const std::vector<BeverageEffect>& newEffects) {
    effects = newEffects;
}

void Beverage::applyToOne(Item& item) const {
    auto* pastry = dynamic_cast<Pastry*>(&item);
    if (!pastry) return;

    for (const auto& effect : effects) {
        const auto& [type, value] = effect;

        if (type == "profit_multiplier")
            pastry->applyMultiplier(value);

        else if (type == "upgrade_discount")
            pastry->applyUpgradeDiscount(value);    }
}
