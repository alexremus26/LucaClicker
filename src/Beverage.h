#ifndef OOP_BEVERAGE_H
#define OOP_BEVERAGE_H

#include "Item.h"
#include <SFML/System/Time.hpp>
#include <vector>
#include <string>
#include <tuple>
#include <memory>

class Beverage : public Item {
private:
    std::vector<std::tuple<std::string, double>> effects;
    std::string targetName;
    std::vector<std::tuple<double, sf::Time, sf::Time>> activeBuffs; // Multiplier, Duration, Remaining

    void applyToOne(Item& item) const;

    [[nodiscard]] double doSellPayout() const override;
    [[nodiscard]] double doDeliveryPayout() const override;
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
    Beverage(std::string name_, double multiplier_, double unlockCost_,
             std::vector<std::tuple<std::string, double>> effects_,
             std::string targetName_ = "ALL");
    Beverage(const Beverage& other);
    ~Beverage() override;
    [[nodiscard]] Item* clone() const override;

    [[nodiscard]] double getUpgradeCost() const override;
    [[nodiscard]] double getSpeedMultiplier() const override;
    static double getBuffMultiplier() ;

    void updateBuffs(sf::Time deltaTime);
    void update(sf::Time time) override;
};

#endif // OOP_BEVERAGE_H
    