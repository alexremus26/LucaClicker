#ifndef OOP_GLOVOPLATFORM_H
#define OOP_GLOVOPLATFORM_H

#include "DeliveryPlatform.h"
#include <iostream>

class GlovoPlatform : public DeliveryPlatform {
private:
    static constexpr double MIN_DURATION = 0.5;     // slowest delivery
    static constexpr double DURATION_SCALE = 0.12;  // scales fastest with income

    double doComputeIncome(const Item& item) const override;
    sf::Time doComputeSpeed(const Item& item) const override;

public:
    GlovoPlatform();
    ~GlovoPlatform() override;

    GlovoPlatform* clone() const override;
};

#endif // OOP_GLOVOPLATFORM_H
