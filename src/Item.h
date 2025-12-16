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
    [[nodiscard]] virtual double doSellPayout() const = 0;
    [[nodiscard]] virtual double doDeliveryPayout() const = 0;

    virtual void doPrint(std::ostream& os) const = 0;
    virtual void doUpgrade() = 0; // de implementat upgrade pentru beverage si sandwich
    virtual void doApplyMultiplier(double multiplier) = 0;
    [[nodiscard]] virtual sf::Time doComputeDuration() const = 0;
    [[nodiscard]] virtual std::string doGetEffectDescription() const = 0;
    virtual void doSetBaseIncome(double newBaseIncome) = 0;
    virtual void doSetUpgradeCost(double newUpgradeCost) = 0;


public:
    Item(std::string name_, double multiplier_, double unlockCost_);
    Item(const Item& other);
    virtual ~Item();
    friend std::ostream& operator<<(std::ostream& ostream, const Item& item);
    [[nodiscard]] virtual Item* clone() const = 0;

    friend void swap(Item &lhs, Item &rhs) noexcept {
        using std::swap;
        swap(lhs.name, rhs.name);
        swap(lhs.multiplier, rhs.multiplier);
        swap(lhs.unlockCost, rhs.unlockCost);
        swap(lhs.level, rhs.level);
    }

    Item& operator=(const Item& other) {
        if (this != &other) {
            const auto copy = other.clone();
            using std::swap;
            swap(*this, *copy);
        }
        return *this;
    }

    // NVI
    [[nodiscard]] double sellPayout() const;
    [[nodiscard]] double deliveryPayout() const;
    void print(std::ostream& os) const;
    void applyMultiplier(double mult);
    void upgrade();
    [[nodiscard]] sf::Time getDuration() const;
    void setBaseIncome(double newBaseIncome);
    void setUpgradeCost(double newUpgradeCost);
    [[nodiscard]] std::string getEffectDescription() const;
    [[nodiscard]] double getUnlockCost() const;
    [[nodiscard]] const std::string& getName() const;

    [[nodiscard]] double getMultiplier() const { return multiplier; }
    void setMultiplier(const double newMultiplier) { multiplier = newMultiplier; }
    [[nodiscard]] int getLevel() const { return level; }
    void setLevel(const int newLevel) { level = newLevel; }


};

#endif //OOP_ITEM_H
