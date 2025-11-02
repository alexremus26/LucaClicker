#ifndef OOP_FOODITEM_H
#define OOP_FOODITEM_H

#include <string>

class FoodItem {
    std::string foodName;
    double baseIncome;
    double upgradeCost;
    double unlockCost;
    double incomeMultiplier;
    double upgradeMultiplier;

public:
    FoodItem(std::string  foodName_, const double baseIncome_, const double upgradeCost_,
             const double incomeMultiplier_, const double upgradeMultiplier_, const double unlockCost_);
    FoodItem(const FoodItem& foodItem);
    ~FoodItem();
    FoodItem& operator=(const FoodItem& foodItem);
    friend std::ostream& operator<<(std::ostream& ostream, const FoodItem& foodItem);

    double getUnlockCost() const;
    const std::string& getFoodName() const;
    double getBaseIncome() const;
    double getUpgradeCost() const;
    double newIncome() const;
    double newUpgradeCost() const;
    void update();
};


#endif //OOP_FOODITEM_H