#ifndef OOP_DELIVERYPLATFORM_H
#define OOP_DELIVERYPLATFORM_H

#include <SFML/System/Time.hpp>
#include "Item.h"

class DeliveryPlatform {
private:
    // NVI
    [[nodiscard]] virtual double doComputeIncome(const Item&) const = 0;
    [[nodiscard]] virtual sf::Time doComputeSpeed(const Item&) const = 0;

public:
    virtual ~DeliveryPlatform();
    [[nodiscard]] virtual DeliveryPlatform* clone() const = 0;
    // NVI
    [[nodiscard]] double computeIncome(const Item&) const;
    [[nodiscard]] sf::Time computeSpeed(const Item&) const;
};

#endif //OOP_DELIVERYPLATFORM_H
