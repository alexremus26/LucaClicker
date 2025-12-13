#ifndef OOP_SANDWICH_H
#define OOP_SANDWICH_H

#include "Item.h"
#include <random>


class Sandwich final : public Item {
private:
    double fastMultiplier;
    double slowMultiplier;
    double fastChance;
    sf::Time baseDuration;
    mutable std::mt19937 rng;

    bool rollFastEffect() const;

    double doSellPayout() const override;
    sf::Time doSellDuration() const override;
    double doDeliveryPayout() const override;
    void doPrint(std::ostream& os) const override;
    void doUpgrade() override;
    void doApplyMultiplier(double multiplier) override;
    [[nodiscard]] sf::Time doComputeDuration() const override;
    [[nodiscard]] std::string doGetEffectDescription() const override;
    void doSetBaseIncome(double) override {}
    void doSetUpgradeCost(double) override {}

public:
    Sandwich(std::string name,
             double unlockCost,
             double baseIncome,
             double upgradeCost,
             double fastMult,
             double slowMult,
             double fastChance,
             sf::Time baseDuration);

    Sandwich(const Sandwich& other);
    ~Sandwich() override = default;

    [[nodiscard]] Item* clone() const override;

    friend void swap(Sandwich& lhs, Sandwich& rhs) noexcept {
        using std::swap;
        swap(static_cast<Item&>(lhs), static_cast<Item&>(rhs));
        swap(lhs.fastMultiplier, rhs.fastMultiplier);
        swap(lhs.slowMultiplier, rhs.slowMultiplier);
        swap(lhs.fastChance, rhs.fastChance);
        swap(lhs.baseDuration, rhs.baseDuration);
        swap(lhs.rng, rhs.rng);
    }

    Sandwich& operator=(Sandwich other) {
        swap(*this, other);
        return *this;
    }
};

#endif // OOP_SANDWICH_H
