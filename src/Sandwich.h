#ifndef OOP_SANDWICH_H
#define OOP_SANDWICH_H

#include "Item.h"
#include <random>
#include <optional>


class Sandwich final : public Item {
private:
    double fastMultiplier;
    double slowMultiplier;
    double fastChance;
    sf::Time baseDuration;
    mutable std::mt19937 rng;

    std::optional<std::tuple<double, sf::Time, sf::Time>> currentSpeedBuff;

    bool rollFastEffect() const;
    double doSellPayout() const override;
    double doDeliveryPayout() const override;
    void doPrint(std::ostream& os) const override;
    void doUpgrade() override;
    void doApplyMultiplier(double multi) override;
    [[nodiscard]] sf::Time doComputeDuration() const override;
    [[nodiscard]] std::string doGetEffectDescription() const override;
    void doUse(std::vector<std::unique_ptr<Item>>& allItems,
               std::queue<std::string>& eventMessages,
               std::mutex& eventMutex) override;
    void doSave(std::ostream& os) const override;
    void doLoad(std::istream& is) override;
    [[nodiscard]] std::string getType() const override;
    void doDrawRaffle(Player &player, double incomePerSecond, std::queue<std::string> &eventMessages, std::mutex &eventMutex) const override;
    [[nodiscard]] double doComputeIncomePerSecond() const override;

public:
    static void registerItem();
    Sandwich(std::string name,
             double unlockCost,
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
        swap(lhs.currentSpeedBuff, rhs.currentSpeedBuff);
    }

    Sandwich& operator=(Sandwich other) {
        swap(*this, other);
        return *this;
    }
    void update(sf::Time time) override;

    [[nodiscard]] double getUpgradeCost() const override;
    [[nodiscard]] double getSpeedMultiplier() const override;
};

#endif // OOP_SANDWICH_H
