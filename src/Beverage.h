#ifndef OOP_BEVERAGE_H
#define OOP_BEVERAGE_H

#include "Item.h"
#include <SFML/System/Time.hpp>
#include <vector>
#include <string>

struct BeverageEffect {
    std::string type;   // "profit_multiplier", "upgrade_discount"
    double value;       // ex 1.5, 0.9
};

class Beverage final : public Item {
private:
    std::vector<BeverageEffect> effects;
    std::string targetName;

    void doPrint(std::ostream& os) const override;
    void doUpgrade() override;
    void doApplyMultiplier(double mult) override;
    [[nodiscard]] sf::Time doGetDuration() const override;
    [[nodiscard]] std::string doGetEffectDescription() const override;
    [[nodiscard]] double doGetBaseIncome() const override;
    [[nodiscard]] double doGetUpgradeCost() const override;
    void doSetBaseIncome(double newBaseIncome) override;
    void doSetUpgradeCost(double newUpgradeCost) override;

public:
    Beverage(std::string name_, double multiplier_, double unlockCost_,
            std::vector<BeverageEffect> effects_, std::string targetName_ = "ALL");
    Beverage(const Beverage& other);
    ~Beverage() override;
    Beverage& operator=(const Beverage& other);
    [[nodiscard]] Item* clone() const override;

    void applyToOne(Item& item) const;
    void setEffects(const std::vector<BeverageEffect>& newEffects);
    [[nodiscard]] const std::vector<BeverageEffect>& getEffects() const;
    [[nodiscard]] const std::string& getTarget() const { return targetName; }
    void setTarget(const std::string& newTarget) { targetName = newTarget; }

};

#endif // OOP_BEVERAGE_H
