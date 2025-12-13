#ifndef OOP_PASTRY_H
#define OOP_PASTRY_H

#include "Item.h"
#include <SFML/System/Time.hpp>

class Pastry : public Item {
private:
    double baseIncome;
    double upgradeCost;
    sf::Time duration;

    sf::Time doSellDuration() const override;
    double doSellPayout() const override;
    double doDeliveryPayout() const override;
    void doPrint(std::ostream &os) const override;
    void doApplyMultiplier(double mult) override;
    void doUpgrade() override;
    [[nodiscard]] sf::Time doComputeDuration() const override;
    [[nodiscard]] std::string doGetEffectDescription() const override;
    void doSetBaseIncome(double newBaseIncome) override;
    void doSetUpgradeCost(double newUpgradeCost) override;

public:
    Pastry(std::string name_, double multiplier_, double unlockCost_,
           double baseIncome_, double upgradeCost_, sf::Time duration_);
    Pastry(const Pastry& other);
    ~Pastry() override;
    [[nodiscard]] Item* clone() const override;

    Pastry& operator=(Pastry other) {
        if (this != &other) {
            using std::swap;
            swap(*this, other);
            return *this;
        }
        return *this;
    }

    friend void swap(Pastry &lhs, Pastry &rhs) noexcept {
        using std::swap;
        swap(static_cast<Item &>(lhs), static_cast<Item &>(rhs));
        swap(lhs.baseIncome, rhs.baseIncome);
        swap(lhs.upgradeCost, rhs.upgradeCost);
        swap(lhs.duration, rhs.duration);
    }


    [[nodiscard]] double getBaseIncome() const;
    [[nodiscard]] double getUpgradeCost() const;
};

#endif // OOP_PASTRY_H
