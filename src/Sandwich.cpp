#include "Sandwich.h"
#include <ostream>

Sandwich::Sandwich(std::string name,
                   const double unlockCost,
                   const double fastMult,
                   const double slowMult,
                   const double fastChance,
                   const sf::Time baseDuration)
    : Item(std::move(name), 1.0, unlockCost),
      fastMultiplier(fastMult),
      slowMultiplier(slowMult),
      fastChance(fastChance),
      baseDuration(baseDuration),
      rng(std::random_device{}())
{}

Sandwich::Sandwich(const Sandwich& other)
    : Item(other),
      fastMultiplier(other.fastMultiplier),
      slowMultiplier(other.slowMultiplier),
      fastChance(other.fastChance),
      baseDuration(other.baseDuration),
      rng(std::random_device{}())
{}

Item* Sandwich::clone() const {
    return new Sandwich(*this);
}

bool Sandwich::rollFastEffect() const {
    std::uniform_real_distribution dist(0.0, 1.0);
    return dist(rng) < fastChance;
}

double Sandwich::doSellPayout() const {
    return 0;
}


double Sandwich::doDeliveryPayout() const {
    return 0.0;
}

void Sandwich::doPrint(std::ostream& os) const {
    os << "Sandwich: " << name
       << " | Level: " << level
       << " | Fast x" << fastMultiplier
       << " | Slow x" << slowMultiplier
       << " | Fast chance: " << fastChance * 100 << "%";
}

void Sandwich::doUpgrade() {
    ++level;

    // fastChance = std::min(1.0, fastChance + 0.02);
    fastMultiplier += 0.1;
    slowMultiplier += 0.05;
}

sf::Time Sandwich::doComputeDuration() const {
    if (rollFastEffect())
        return baseDuration * static_cast<float>(slowMultiplier);
    else
        return baseDuration * static_cast<float>(fastMultiplier);
}


std::string Sandwich::doGetEffectDescription() const {
    return "Random speed: fast x" + std::to_string(fastMultiplier) +
           " or slow x" + std::to_string(slowMultiplier);
}

void Sandwich::doApplyMultiplier(const double mult) {
    multiplier *= mult;
}
