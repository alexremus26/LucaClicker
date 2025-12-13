#include "Item.h"
#include <iostream>


Item::Item(std::string name_, const double multiplier_, const double unlockCost_)
    : name(std::move(name_)), multiplier(multiplier_), unlockCost(unlockCost_), level(1){}
Item::Item(const Item& other)
    : name(other.name),  multiplier(other.multiplier), unlockCost(other.unlockCost), level(other.level){}
Item::~Item() {
    std::cout << "Item-ul " << name << " a fost distrus!\n";
}


std::ostream& operator<<(std::ostream& ostream, const Item& item) {
    item.print(ostream);
    return ostream;
}


double Item::sellPayout() const {
    return doSellPayout();
}

sf::Time Item::sellDuration() const {
    return doSellDuration();
}

double Item::deliveryPayout() const {
    return doDeliveryPayout();
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


int Item::getLevel() const {
    return level;
}

void Item::applyMultiplier(const double mult) {
    doApplyMultiplier(mult);
}

void Item::upgrade() {
    doUpgrade();
}

sf::Time Item::getDuration() const {
    return doComputeDuration();
}



std::string Item::getEffectDescription() const {
    return doGetEffectDescription();
}

void Item::setBaseIncome(const double newBaseIncome) {
    doSetBaseIncome(newBaseIncome);
}

void Item::setUpgradeCost(const double newUpgradeCost) {
    doSetUpgradeCost(newUpgradeCost);
}





