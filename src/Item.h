#ifndef OOP_ITEM_H
#define OOP_ITEM_H

#include <string>
#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <SFML/System/Time.hpp>

class Item {
protected:
    std::string name;
    double multiplier;
    double unlockCost;
    double useCost;
    int level;

private:
    [[nodiscard]] virtual double doSellPayout() const = 0;
    [[nodiscard]] virtual double doDeliveryPayout() const = 0;

    virtual void doPrint(std::ostream& os) const = 0;
    virtual void doUpgrade() = 0;
    virtual void doApplyMultiplier(double multiplier) = 0;
    [[nodiscard]] virtual sf::Time doComputeDuration() const = 0;
    [[nodiscard]] virtual std::string doGetEffectDescription() const = 0;

    virtual void doUse(std::vector<std::unique_ptr<Item>>& allItems,
                       std::queue<std::string>& eventMessages,
                       std::mutex& eventMutex) = 0;

protected:
    virtual void doSave(std::ostream& os) const = 0;
    virtual void doLoad(std::istream& is) = 0;

public:
    Item(std::string name_, double multiplier_, double unlockCost_);
    Item(const Item& other);
    virtual ~Item();

    friend std::ostream& operator<<(std::ostream& ostream, const Item& item);

    [[nodiscard]] virtual Item* clone() const = 0;
    [[nodiscard]] virtual std::string getType() const = 0;

    friend void swap(Item& lhs, Item& rhs) noexcept {
        using std::swap;
        swap(lhs.name, rhs.name);
        swap(lhs.multiplier, rhs.multiplier);
        swap(lhs.unlockCost, rhs.unlockCost);
        swap(lhs.useCost, rhs.useCost);
        swap(lhs.level, rhs.level);
    }

    Item& operator=(const Item& other) {
        if (this != &other) {
            std::unique_ptr<Item> copy(other.clone());
            swap(*this, *copy);
        }
        return *this;
    }

    void save(std::ostream& os) const;
    void load(std::istream& is);

    [[nodiscard]] double sellPayout() const;
    [[nodiscard]] double deliveryPayout() const;

    void print(std::ostream& os) const;
    void applyMultiplier(double mult);
    void upgrade();

    [[nodiscard]] sf::Time getDuration() const;
    [[nodiscard]] std::string getEffectDescription() const;

    void use(std::vector<std::unique_ptr<Item>>& allItems,
             std::queue<std::string>& eventMessages,
             std::mutex& eventMutex);

    [[nodiscard]] double getUnlockCost() const;
    [[nodiscard]] double getUseCost() const;
    [[nodiscard]] virtual double getUpgradeCost() const = 0;

    [[nodiscard]] const std::string& getName() const;

    virtual void update(sf::Time time);
    [[nodiscard]] virtual double getSpeedMultiplier() const;
    [[nodiscard]] virtual bool isUsable() const;

    virtual void applyEffect(const std::string& type, double value);
};

#endif // OOP_ITEM_H
