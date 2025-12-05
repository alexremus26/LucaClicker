#ifndef OOP_PASTRY_H
#define OOP_PASTRY_H

#include "Item.h"
#include <SFML/System/Time.hpp>

class GameManager;

class Pastry : public Item {
private:
    void doPrint(std::ostream &os) const override;
    void doApplyEffect() const override;
    void doApplyMultiplier(double mult) override;
    void doUpgrade() override;
    [[nodiscard]] double doCalculateRevenue() const override;
    [[nodiscard]] sf::Time doGetDuration() const override;
    [[nodiscard]] std::string doGetEffectDescription() const override;

public:

    Pastry(std::string name_, double baseIncome_, double upgradeCost_,
           double unlockCost_, double multiplier_, sf::Time duration_);
    Pastry(const Pastry& other);
    ~Pastry() override;
    Pastry& operator=(const Pastry& other);
    [[nodiscard]] Item* clone() const override;
};

#endif //OOP_PASTRY_H