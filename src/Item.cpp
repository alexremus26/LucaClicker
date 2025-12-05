#include "Item.h"
#include <iostream>


Item::Item(std::string name_, const double baseIncome_, const double upgradeCost_, const double unlockCost_, double multiplier_, sf::Time duration_)
    : name(std::move(name_)), baseIncome(baseIncome_), upgradeCost(upgradeCost_),
      unlockCost(unlockCost_), multiplier(multiplier_), level(1), duration(duration_){}
Item::Item(const Item& other)
    : name(other.name), baseIncome(other.baseIncome), upgradeCost(other.upgradeCost),
      unlockCost(other.unlockCost), multiplier(other.multiplier), level(other.level), duration(other.duration) {}
Item::~Item() {
    std::cout << "Item-ul " << name << " a fost distrus!\n";
}
Item& Item::operator=(const Item& other) {
    if (this != &other) {
        name = other.name;
        baseIncome = other.baseIncome;
        upgradeCost = other.upgradeCost;
        unlockCost = other.unlockCost;
        level = other.level;
    }
    return *this;
}
std::ostream& operator<<(std::ostream& ostream, const Item& item) {
    item.print(ostream);
    return ostream;
}

void Item::print(std::ostream& os) const {
    doPrint(os);
}

double Item::getUnlockCost() const {
    return unlockCost;
}

const std::string& Item::getName() const {
    return name;
}

double Item::getUpgradeCost() const {
    return upgradeCost;
}

int Item::getLevel() const {
    return level;
}

double Item::getBaseIncome() const {
    return baseIncome;
}

void Item::setBaseIncome(const double newBaseIncome) {
    baseIncome = newBaseIncome;
}

void Item::setUpgradeCost(const double newUpgradeCost) {
    upgradeCost = newUpgradeCost;
}

void Item::applyEffect() const {
    doApplyEffect();
}

void Item::applyMultiplier(const double mult) {
    doApplyMultiplier(mult);
}

void Item::upgrade() {
    doUpgrade();
}

double Item::calculateRevenue() const {
    return doCalculateRevenue();
}

sf::Time Item::getDuration() const {
    return doGetDuration();
}

std::string Item::getEffectDescription() const {
    return doGetEffectDescription();
}






