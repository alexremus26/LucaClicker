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
    double doDeliveryPayout() const override;
    void doPrint(std::ostream& os) const override;
    void doUpgrade() override;
    void doApplyMultiplier(double mult) override;
    [[nodiscard]] sf::Time doComputeDuration() const override;
    [[nodiscard]] std::string doGetEffectDescription() const override;
    void doSetBaseIncome(double) override {}
    void doSetUpgradeCost(double) override {}
    void doUse(std::vector<std::unique_ptr<Item>>& allItems,
               std::vector<std::tuple<double, sf::Time, sf::Time>>& activeSpeedBuffs,
               std::queue<std::string>& eventMessages,
               std::mutex& eventMutex) override;
    void doSave(std::ostream& os) const override;
    void doLoad(std::istream& is) override;
    [[nodiscard]] std::string getType() const override;

public:
    Sandwich(std::string name,
             double unlockCost,
             double fastMult,
             double slowMult,
             double fastChance,
             sf::Time baseDuration);
    Sandwich(const Sandwich& other);
    ~Sandwich() override = default;
    [[nodiscard]] Item* clone() const override;

    [[nodiscard]] double rollMultiplier() const;

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
    [[nodiscard]] double getUpgradeCost() const override;
};

#endif // OOP_SANDWICH_H
