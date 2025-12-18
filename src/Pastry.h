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
    void doApplyMultiplier(double mult) override;
    void doUpgrade() override;
    [[nodiscard]] sf::Time doComputeDuration() const override;
    [[nodiscard]] std::string doGetEffectDescription() const override;
    void doUse(std::vector<std::unique_ptr<Item>>& allItems,
               std::queue<std::string>& eventMessages,
               std::mutex& eventMutex) override;
    void doSave(std::ostream& os) const override;
    void doLoad(std::istream& is) override;
    [[nodiscard]] std::string getType() const override;

public:
    Pastry(std::string name_, double multiplier_, double unlockCost_,
           double baseIncome_, double upgradeCost_, sf::Time duration_);
    Pastry(const Pastry& other);
    ~Pastry() override;
    [[nodiscard]] Item* clone() const override;

    Pastry& operator=(Pastry other) {
        if (this != &other) {
            using std::swap;
            swap(*this, other);
            return *this;
        }
        return *this;
    }

    friend void swap(Pastry &lhs, Pastry &rhs) noexcept {
        using std::swap;
        swap(static_cast<Item &>(lhs), static_cast<Item &>(rhs));
        swap(lhs.baseIncome, rhs.baseIncome);
        swap(lhs.upgradeCost, rhs.upgradeCost);
        swap(lhs.duration, rhs.duration);
    }
    void applyUpgradeDiscount(double factor);
    [[nodiscard]] double getUpgradeCost() const override;
    [[nodiscard]] bool isUsable() const override;
    void applyEffect(const std::string& type, double value) override;
};

#endif // OOP_PASTRY_H
