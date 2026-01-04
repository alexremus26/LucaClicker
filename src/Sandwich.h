#ifndef OOP_SANDWICH_H
#define OOP_SANDWICH_H

#include "Item.h"
#include <random>
#include <optional>


class Sandwich : public Item {
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

    void update(sf::Time time) override;

    [[nodiscard]] double getUpgradeCost() const override;
    [[nodiscard]] double getSpeedMultiplier() const override;
};

#endif // OOP_SANDWICH_H
