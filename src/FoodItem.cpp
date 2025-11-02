#include "FoodItem.h"
#include <iostream>

FoodItem::FoodItem(std::string  foodName_, const double baseIncome_, const double upgradeCost_, const double incomeMultiplier_, const double upgradeMultiplier_, const double unlockCost_)
         :foodName(std::move(foodName_)), baseIncome(baseIncome_), upgradeCost(upgradeCost_), unlockCost(unlockCost_), incomeMultiplier(incomeMultiplier_), upgradeMultiplier(upgradeMultiplier_){}

FoodItem::FoodItem(const FoodItem& foodItem)
         :foodName(foodItem.foodName), baseIncome(foodItem.baseIncome), upgradeCost(foodItem.upgradeCost), unlockCost(foodItem.unlockCost), incomeMultiplier(foodItem.incomeMultiplier), upgradeMultiplier(foodItem.upgradeMultiplier){}

FoodItem::~FoodItem(){std::cout<<"FoodItem-ul "<< foodName <<" a fost distrus!  \n";}

FoodItem &FoodItem::operator=(const FoodItem &foodItem) {
    foodName = foodItem.foodName;
    baseIncome = foodItem.baseIncome;
    upgradeCost = foodItem.upgradeCost;
    incomeMultiplier = foodItem.incomeMultiplier;
    upgradeMultiplier = foodItem.upgradeMultiplier;
    unlockCost = foodItem.unlockCost;
    return *this;
}

std::ostream &operator<<(std::ostream &ostream, const FoodItem &foodItem) {
    ostream << foodItem.foodName << " a-> Pret:" << foodItem.baseIncome << "  CostUpgrade:" << foodItem.upgradeCost <<
            "  UnlockCost:" << foodItem.unlockCost << "  MultiplicatorPret:" << foodItem.incomeMultiplier <<
            "  Multiplicator upgrade:" << foodItem.upgradeMultiplier << std::endl;
    return ostream;
}

const std::string &FoodItem::getFoodName() const { return foodName; }
double FoodItem::getBaseIncome() const { return baseIncome; }
double FoodItem::getUpgradeCost() const { return upgradeCost; }
double FoodItem::getUnlockCost() const { return unlockCost; }

double FoodItem::newIncome() const {
    return baseIncome * incomeMultiplier + baseIncome;
}

double FoodItem::newUpgradeCost() const {
    return upgradeMultiplier * upgradeCost + upgradeCost;
}

void FoodItem::update() {
    upgradeCost = newUpgradeCost();
    baseIncome = newIncome();
}







