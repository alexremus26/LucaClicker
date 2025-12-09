#ifndef OOP_ITEM_H
#define OOP_ITEM_H

#include <string>
#include <SFML/System/Time.hpp>

class Item {
protected:
    std::string name;
    double multiplier;
    double unlockCost;
    int level;

private:
    // NVI
    virtual void doPrint(std::ostream& os) const = 0;
    virtual void doUpgrade() = 0;
    virtual void doApplyMultiplier(double mult) = 0;
    [[nodiscard]] virtual sf::Time doGetDuration() const = 0;    // DE ADAUGAT DURATA DE SELL
    [[nodiscard]] virtual std::string doGetEffectDescription() const = 0; // folosit principal de bev pe viitor la extra
    [[nodiscard]] virtual double doGetBaseIncome() const = 0;
    [[nodiscard]] virtual double doGetUpgradeCost() const = 0;
    virtual void doSetBaseIncome(double newBaseIncome) = 0;
    virtual void doSetUpgradeCost(double newUpgradeCost) = 0;

public:
    Item(std::string name_, double multiplier_, double unlockCost_);
    Item(const Item& other);
    virtual ~Item();
    Item& operator=(const Item& other);
    friend std::ostream& operator<<(std::ostream& ostream, const Item& item);
    [[nodiscard]] virtual Item* clone() const = 0;

    // NVI
    void print(std::ostream& os) const;
    void applyMultiplier(double mult);
    void upgrade();
    [[nodiscard]] sf::Time getDuration() const;
    [[nodiscard]] double getBaseIncome() const;
    [[nodiscard]] double getUpgradeCost() const;
    void setBaseIncome(double newBaseIncome);
    void setUpgradeCost(double newUpgradeCost);
    [[nodiscard]] std::string getEffectDescription() const;
    [[nodiscard]] double getUnlockCost() const;
    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] int getLevel() const;
};

#endif //OOP_ITEM_H
