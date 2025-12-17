#include "Item.h"
#include <iostream>


Item::Item(std::string name_, const double multiplier_, const double unlockCost_)
    : name(std::move(name_)), multiplier(multiplier_), unlockCost(unlockCost_), useCost(0.0), level(1){}
Item::Item(const Item& other)
    : name(other.name),  multiplier(other.multiplier), unlockCost(other.unlockCost), useCost(other.useCost), level(other.level){}
Item::~Item() {
    std::cout << "Item-ul " << name << " a fost distrus!\n";
}

std::ostream& operator<<(std::ostream& ostream, const Item& item) {
    item.print(ostream);
    return ostream;
}

void Item::save(std::ostream& os) const {
    doSave(os);
}

void Item::load(std::istream& is) {
    doLoad(is);
}

double Item::sellPayout() const {
    return doSellPayout();
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

double Item::getUseCost() const {
    return useCost;
}

void Item::use(std::vector<std::unique_ptr<Item>>& allItems,
             std::vector<std::tuple<double, sf::Time, sf::Time>>& activeSpeedBuffs,
             std::queue<std::string>& eventMessages,
             std::mutex& eventMutex) {
    doUse(allItems, activeSpeedBuffs, eventMessages, eventMutex);
}





