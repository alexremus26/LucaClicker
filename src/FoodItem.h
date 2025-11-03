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
    FoodItem(std::string  foodName_, double baseIncome_, double upgradeCost_,
             double incomeMultiplier_, double upgradeMultiplier_, double unlockCost_);
    FoodItem(const FoodItem& foodItem);
    ~FoodItem();
    FoodItem& operator=(const FoodItem& foodItem);
    friend std::ostream& operator<<(std::ostream& ostream, const FoodItem& foodItem);

    [[nodiscard]] double getUnlockCost() const;
    [[nodiscard]] const std::string& getFoodName() const;
    [[nodiscard]] double getBaseIncome() const;
    [[nodiscard]] double getUpgradeCost() const;
    [[nodiscard]] double newIncome() const;
    [[nodiscard]] double newUpgradeCost() const;
    void setBaseIncome(double newBaseIncome);
    void setUpgradeCost(double newUpgradeCost);
    void update();
};


#endif //OOP_FOODITEM_H