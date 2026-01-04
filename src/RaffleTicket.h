#ifndef OOP_RAFFLETICKET_H
#define OOP_RAFFLETICKET_H

#include "Item.h"
#include <random>
#include <SFML/System/Time.hpp>

class RaffleTicket : public Item {
private:
    sf::Time minDuration;
    sf::Time maxDuration;
    mutable std::mt19937 rng;

    [[nodiscard]] double doSellPayout() const override;
    [[nodiscard]] double doDeliveryPayout() const override;
    void doPrint(std::ostream& os) const override;
    void doUpgrade() override;

    [[nodiscard]] sf::Time doComputeDuration() const override;
    [[nodiscard]] std::string doGetEffectDescription() const override;
    void doSave(std::ostream& os) const override;
    void doLoad(std::istream& is) override;
    [[nodiscard]] std::string getType() const override;
    void doUse(std::vector<std::unique_ptr<Item>> &allItems,
                    std::queue<std::string> &eventMessages,
                    std::mutex &eventMutex) override;

    void doDrawRaffle(Player &player, double incomePerSecond,
                      std::queue<std::string> &eventMessages,
                      std::mutex &eventMutex) const override;
    [[nodiscard]] double doComputeIncomePerSecond() const override;

public:

    static void registerItem();
    RaffleTicket(std::string name, double unlockCost, sf::Time minDuration, sf::Time maxDuration);
    RaffleTicket(const RaffleTicket& other);
    ~RaffleTicket() override = default;
    [[nodiscard]] Item* clone() const override;

    friend void swap(RaffleTicket& lhs, RaffleTicket& rhs) noexcept {
        using std::swap;
        swap(static_cast<Item&>(lhs), static_cast<Item&>(rhs));
        swap(lhs.minDuration, rhs.minDuration);
        swap(lhs.maxDuration, rhs.maxDuration);
        swap(lhs.rng, rhs.rng);
    }

    RaffleTicket& operator=(RaffleTicket other) {
        swap(*this, other);
        return *this;
    }
    [[nodiscard]] double getUpgradeCost() const override;

    [[nodiscard]] bool isUsable() const override;
};

#endif // OOP_RAFFLETICKET_H
