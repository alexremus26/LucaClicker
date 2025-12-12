#ifndef OOP_WOLTPLATFORM_H
#define OOP_WOLTPLATFORM_H

#include "DeliveryPlatform.h"

class WoltPlatform : public DeliveryPlatform {
private:
    static constexpr double MIN_DURATION = 0.4;
    static constexpr double DURATION_SCALE = 0.10;

    [[nodiscard]] double doComputeIncome(const Item& item) const override;
    [[nodiscard]] sf::Time doComputeSpeed(const Item& item) const override;

public:
    WoltPlatform();
    ~WoltPlatform() override;

    [[nodiscard]] WoltPlatform* clone() const override;
};

#endif //OOP_WOLTPLATFORM_H
