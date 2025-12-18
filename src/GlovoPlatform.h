#ifndef OOP_GLOVOPLATFORM_H
#define OOP_GLOVOPLATFORM_H

#include "DeliveryPlatform.h"

class GlovoPlatform : public DeliveryPlatform {
private:
    static constexpr double MIN_DURATION = 1;
    static constexpr double DURATION_SCALE = 0.12;

    [[nodiscard]] IncomeResult doComputeIncome(const Item& item) const override;
    [[nodiscard]] sf::Time doComputeSpeed(const Item& item) const override;

public:
    GlovoPlatform();
    ~GlovoPlatform() override;

    GlovoPlatform* clone() const override;
};

#endif // OOP_GLOVOPLATFORM_H
