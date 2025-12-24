#ifndef OOP_BEVERAGE_H
#define OOP_BEVERAGE_H

#include "Item.h"
#include <SFML/System/Time.hpp>
#include <vector>
#include <string>
#include <tuple>
#include <memory>

class Beverage final : public Item {
private:
    std::vector<std::tuple<std::string, double>> effects;
    std::string targetName;
    std::vector<std::tuple<double, sf::Time, sf::Time>> activeBuffs; // Multiplier, Duration, Remaining

    void applyToOne(Item& item) const;

    [[nodiscard]] double doSellPayout() const override;
    [[nodiscard]] double doDeliveryPayout() const override;
    void doPrint(std::ostream& os) const override;
    void doUpgrade() override;
    void doApplyMultiplier(double mult) override;
    [[nodiscard]] sf::Time doComputeDuration() const override;
    [[nodiscard]] std::string doGetEffectDescription() const override;
    void doUse(std::vector<std::unique_ptr<Item>>& allItems,
               std::queue<std::string>& eventMessages,
               std::mutex& eventMutex) override;
    void doSave(std::ostream& os) const override;
    void doLoad(std::istream& is) override;
    [[nodiscard]] std::string getType() const override;
    void doDrawRaffle(Player &player, double incomePerSecond, std::queue<std::string> &eventMessages, std::mutex &eventMutex) const override;

public:
    static void registerItem();
    Beverage(std::string name_, double multiplier_, double unlockCost_,
             std::vector<std::tuple<std::string, double>> effects_,
             std::string targetName_ = "ALL");
    Beverage(const Beverage& other);
    ~Beverage() override;
    [[nodiscard]] Item* clone() const override;

    friend void swap(Beverage &lhs, Beverage &rhs) noexcept {
        using std::swap;
        swap(static_cast<Item &>(lhs), static_cast<Item &>(rhs));
        swap(lhs.effects, rhs.effects);
        swap(lhs.targetName, rhs.targetName);
        swap(lhs.activeBuffs, rhs.activeBuffs);
    }

    Beverage & operator=(Beverage other) {
        if (this != &other) {
            using std::swap;
            swap(*this, other);
            }
            return *this;
        }
    [[nodiscard]] double getUpgradeCost() const override;
    void updateBuffs(sf::Time deltaTime);
    static double getBuffMultiplier() ;
    [[nodiscard]] std::string& getTargetName();

    void update(sf::Time time) override;
    [[nodiscard]] double getSpeedMultiplier() const override;
    [[nodiscard]] double doComputeIncomePerSecond() const override;
};

#endif // OOP_BEVERAGE_H
    