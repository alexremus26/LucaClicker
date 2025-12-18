#ifndef OOP_DELIVERYPLATFORM_H
#define OOP_DELIVERYPLATFORM_H

#include <SFML/System/Time.hpp>
#include <random>
#include <string>
#include "Item.h"

struct IncomeResult {
    double income;
    std::string message;
};

class DeliveryPlatform {
protected:
    double critChance;
    double critMultiplier;
    mutable std::mt19937 rng;

private:
    // NVI
    [[nodiscard]] virtual IncomeResult doComputeIncome(const Item&) const = 0;
    [[nodiscard]] virtual sf::Time doComputeSpeed(const Item&) const = 0;

public:
    explicit DeliveryPlatform(double critChance, double critMultiplier);
    virtual ~DeliveryPlatform();
    [[nodiscard]] virtual DeliveryPlatform* clone() const = 0;

    [[nodiscard]] virtual bool rollCrit() const;

    // NVI
    [[nodiscard]] IncomeResult computeIncome(const Item&) const;
    [[nodiscard]] sf::Time computeSpeed(const Item&) const;
};


#endif //OOP_DELIVERYPLATFORM_H
