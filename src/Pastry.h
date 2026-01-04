#ifndef OOP_PASTRY_H
#define OOP_PASTRY_H

#include "Item.h"
#include <SFML/System/Time.hpp>

class Pastry : public Item {
private:
    double baseIncome;
    double upgradeCost;
    sf::Time duration;

    [[nodiscard]] double doSellPayout() const override;
    [[nodiscard]] double doDeliveryPayout() const override;
    void doPrint(std::ostream &os) const override;
    void doApplyMultiplier(double multi) override;
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
    Pastry(std::string name_, double multiplier_, double unlockCost_,
           double baseIncome_, double upgradeCost_, sf::Time duration_);
    Pastry(const Pastry& other);
    ~Pastry() override;
    [[nodiscard]] Item* clone() const override;

    void applyUpgradeDiscount(double factor);
    [[nodiscard]] bool isUsable() const override;
    void applyEffect(const std::string& type, double value) override;

    [[nodiscard]] double getUpgradeCost() const override;
};

#endif // OOP_PASTRY_H
