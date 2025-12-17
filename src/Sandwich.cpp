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
{
    useCost = unlockCost / 2;
}

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
    os << "Use Cost: " << getUseCost()
       << " | Level: " << level
       << " | Fast x" << fastMultiplier
       << " | Slow x" << slowMultiplier
       << " | Fast chance: " << fastChance * 100 << "%"
       << " | Slow chance: " << 100 - fastChance * 100 << "%";
}

void Sandwich::doUpgrade() {
    ++level;
    useCost *= 1.5;

    fastChance = std::min(1.0, fastChance + 0.01);
    fastMultiplier += 0.05;
    if(slowMultiplier > 0.1)
        slowMultiplier -= 0.01;
}

sf::Time Sandwich::doComputeDuration() const {
    return baseDuration;
}


std::string Sandwich::doGetEffectDescription() const {
    return "Random speed: fast x" + std::to_string(fastMultiplier) +
           " or slow x" + std::to_string(slowMultiplier);
}

void Sandwich::doApplyMultiplier(const double mult) {
    multiplier *= mult;
}

void Sandwich::doUse(std::vector<std::unique_ptr<Item>>& allItems,
                   std::vector<std::tuple<double, sf::Time, sf::Time>>& activeSpeedBuffs,
                   std::queue<std::string>& eventMessages,
                   std::mutex& eventMutex) {

    (void)allItems;
    (void)activeSpeedBuffs;
    (void)eventMessages;
    (void)eventMutex;
}

std::string Sandwich::getType() const {
    return "Sandwich";
}

double Sandwich::rollMultiplier() const {
    if (rollFastEffect()) {
        return fastMultiplier;
    } else {
        return slowMultiplier;
    }
}

double Sandwich::getUpgradeCost() const {
    return 0.0;
}

void Sandwich::doSave(std::ostream& os) const {
    os << "type: Sandwich\n";
    os << "name: " << name << "\n";
    os << "multiplier: " << multiplier << "\n";
    os << "unlockCost: " << unlockCost << "\n";
    os << "useCost: " << useCost << "\n";
    os << "level: " << level << "\n";
    os << "fastMultiplier: " << fastMultiplier << "\n";
    os << "slowMultiplier: " << slowMultiplier << "\n";
    os << "fastChance: " << fastChance << "\n";
    os << "baseDuration: " << baseDuration.asSeconds() << "\n";
}

void Sandwich::doLoad(std::istream& is) {
    std::string line;
    std::string key;
    std::string value;

    auto getKV = [&](std::string& k, std::string& v) {
        if (!std::getline(is, line)) return false;
        if (line.empty()) return false;
        size_t pos = line.find(':');
        if (pos == std::string::npos) return false;
        k = line.substr(0, pos);
        v = line.substr(pos + 2);
        return true;
    };

    while (getKV(key, value)) {

        if (key == "name") name = value;
        else if (key == "multiplier") multiplier = std::stod(value);
        else if (key == "unlockCost") unlockCost = std::stod(value);
        else if (key == "useCost") useCost = std::stod(value);
        else if (key == "level") level = std::stoi(value);
        else if (key == "fastMultiplier") fastMultiplier = std::stod(value);
        else if (key == "slowMultiplier") slowMultiplier = std::stod(value);
        else if (key == "fastChance") fastChance = std::stod(value);
        else if (key == "baseDuration") baseDuration = sf::seconds(std::stof(value));
        else if (key == "type") {}
        else {
        }
    }
}
