#ifndef OOP_PASTRY_H
#define OOP_PASTRY_H

#include "Item.h"
#include <SFML/System/Time.hpp>

class Pastry : public Item {
private:
    double baseIncome;
    double upgradeCost;
    sf::Time duration;

    void doPrint(std::ostream &os) const override;
    void doApplyMultiplier(double mult) override;
    void doUpgrade() override;
    [[nodiscard]] sf::Time doGetDuration() const override;
    [[nodiscard]] std::string doGetEffectDescription() const override;
    [[nodiscard]] double doGetBaseIncome() const override;
    [[nodiscard]] double doGetUpgradeCost() const override;
    void doSetBaseIncome(double newBaseIncome) override;
    void doSetUpgradeCost(double newUpgradeCost) override;

public:
    Pastry(std::string name_, double multiplier_, double unlockCost_,
           double baseIncome_, double upgradeCost_, sf::Time duration_);
    Pastry(const Pastry& other);
    ~Pastry() override;
    Pastry& operator=(const Pastry& other);
    [[nodiscard]] Item* clone() const override;

    friend void swap(Pastry &lhs, Pastry &rhs) noexcept {
        using std::swap;
        swap(static_cast<Item &>(lhs), static_cast<Item &>(rhs));
        swap(lhs.baseIncome, rhs.baseIncome);
        swap(lhs.upgradeCost, rhs.upgradeCost);
        swap(lhs.duration, rhs.duration);
    }
};

#endif // OOP_PASTRY_H
