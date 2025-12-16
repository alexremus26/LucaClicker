#ifndef OOP_BEVERAGE_H
#define OOP_BEVERAGE_H

#include "Item.h"
#include <SFML/System/Time.hpp>
#include <vector>
#include <string>
#include <tuple>
#include <memory>

using BeverageEffect = std::tuple<std::string, double>;

class Beverage final : public Item {
private:
    std::vector<BeverageEffect> effects;
    std::string targetName;
    double useCost;

    [[nodiscard]] double doSellPayout() const override;
    [[nodiscard]] double doDeliveryPayout() const override;
    void doPrint(std::ostream& os) const override;
    void doUpgrade() override;
    void doApplyMultiplier(double mult) override;
    [[nodiscard]] sf::Time doComputeDuration() const override;
    [[nodiscard]] std::string doGetEffectDescription() const override;
    void doSetBaseIncome(double newBaseIncome) override;
    void doSetUpgradeCost(double newUpgradeCost) override;
    [[nodiscard]] std::string getType() const override;

public:
    Beverage(std::string name_, double multiplier_, double unlockCost_,
             std::vector<BeverageEffect> effects_,
             std::string targetName_ = "ALL");
    Beverage(const Beverage& other);
    ~Beverage() override;
    [[nodiscard]] Item* clone() const override;

    friend void swap(Beverage &lhs, Beverage &rhs) noexcept {
        using std::swap;
        swap(static_cast<Item &>(lhs), static_cast<Item &>(rhs));
        swap(lhs.effects, rhs.effects);
        swap(lhs.targetName, rhs.targetName);
        swap(lhs.useCost, rhs.useCost);
    }

    Beverage & operator=(Beverage other) {
        if (this != &other) {
            using std::swap;
            swap(*this, other);
        }
        return *this;
    }
    void activate(const std::vector<std::unique_ptr<Item>>& allItems);
    void applyToOne(Item& item) const;
    void setEffects(const std::vector<BeverageEffect>& newEffects);
    [[nodiscard]] const std::vector<BeverageEffect>& getEffects() const;
    [[nodiscard]] const std::string& getTarget() const { return targetName; }
    void setTarget(const std::string& newTarget) { targetName = newTarget; }
    [[nodiscard]] double getUseCost() const { return useCost; }
};

#endif // OOP_BEVERAGE_H
    