#ifndef OOP_ITEM_H
#define OOP_ITEM_H

#include <string>
#include <SFML/System/Time.hpp>
#include "GameManager.h"

class Item {
protected:
    std::string name;
    double baseIncome;
    double upgradeCost;
    double unlockCost;
    double multiplier;
    int level;
    sf::Time duration;

    // NVI: privat
private:
    virtual void doPrint(std::ostream& os) const = 0;
    virtual void doApplyEffect() const = 0;
    virtual void doUpgrade() = 0;
    virtual void doApplyMultiplier(double mult) = 0;
    [[nodiscard]] virtual double doCalculateRevenue() const = 0;
    [[nodiscard]] virtual sf::Time doGetDuration() const = 0;
    [[nodiscard]] virtual std::string doGetEffectDescription() const = 0;

public:
    Item(std::string name_, double baseIncome_, double upgradeCost_,double multiplier, double unlockCost_, sf::Time duration_);
    Item(const Item& other);
    virtual ~Item();
    Item& operator=(const Item& other);
    friend std::ostream& operator<<(std::ostream& ostream, const Item& item);

    [[nodiscard]] virtual Item* clone() const = 0;

    // NVI public
    void print(std::ostream& os) const;
    void applyEffect() const;
    void applyMultiplier(double mult);
    void upgrade();
    [[nodiscard]] double calculateRevenue() const;
    [[nodiscard]] sf::Time getDuration() const;
    [[nodiscard]] std::string getEffectDescription() const;

    [[nodiscard]] double getUnlockCost() const;
    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] double getUpgradeCost() const;
    [[nodiscard]] int getLevel() const;
    [[nodiscard]] double getBaseIncome() const;
    void setBaseIncome(double newBaseIncome);
    void setUpgradeCost(double newUpgradeCost);
};

#endif //OOP_ITEM_H