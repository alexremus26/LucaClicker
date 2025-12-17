#include "Beverage.h"
#include "Pastry.h"
#include <iostream>
#include <sstream>

Beverage::Beverage(std::string name_, const double multiplier_, const double unlockCost_,
                   std::vector<BeverageEffect> effects_,
                   std::string targetName_)
    : Item(std::move(name_), multiplier_, unlockCost_),
      effects(std::move(effects_)),
      targetName(std::move(targetName_)) {
    useCost = unlockCost / 2;
}

Beverage::Beverage(const Beverage& other)
    : Item(other),
      effects(other.effects),
      targetName(other.targetName) {}

Beverage::~Beverage() {
    std::cout << "Beverage " << name << " destroyed\n";
}


Item* Beverage::clone() const {
    return new Beverage(*this);
}

double Beverage::doSellPayout() const {
    return 0.0;
}

double Beverage::doDeliveryPayout() const {
    return 0.0;
}

void Beverage::doPrint(std::ostream& os) const {
    os << "Use Cost: " << getUseCost()
       << " | Level: " << level
       << " | Target: " << targetName
       << " | Effects: " << getEffectDescription();
}

void Beverage::doApplyMultiplier(const double mult) {
    multiplier *= mult;
}

void Beverage::doUpgrade() {
    level++;
    useCost *= 2.0;
}

sf::Time Beverage::doComputeDuration() const {
    return sf::seconds(0.f);
}

std::string Beverage::doGetEffectDescription() const {
    std::ostringstream os;
    bool first = true;

    for (const auto& effect : effects) {
        const auto& [type, value] = effect;

        if (!first) os << ", ";
        first = false;

        if (type == "profit_multiplier")
            os << "Profit x" << value;
        else if (type == "upgrade_discount")
            os << "Upgrade cost x" << value;
        else
            os << type << "(" << value << ")";
    }

    if (first) os << "No effects";
    return os.str();
}

void Beverage::doSetBaseIncome(double) {}
void Beverage::doSetUpgradeCost(double) {}

void Beverage::doUse(std::vector<std::unique_ptr<Item>>& allItems,
                   std::vector<std::tuple<double, sf::Time, sf::Time>>& activeSpeedBuffs,
                   std::queue<std::string>& eventMessages,
                   std::mutex& eventMutex) {

    if (targetName == "all") {
        for (auto& it : allItems)
            applyToOne(*it);
    } else {
        for (auto& it : allItems)
            if (it->getName() == targetName)
                applyToOne(*it);
    }

    (void)activeSpeedBuffs;
    (void)eventMessages;
    (void)eventMutex;
}

void Beverage::applyToOne(Item& item) const {
    auto* pastry = dynamic_cast<Pastry*>(&item);
    if (!pastry) return;

    for (const auto& effect : effects) {
        const auto& [type, value] = effect;

        if (type == "profit_multiplier")
            pastry->applyMultiplier(value);

        else if (type == "upgrade_discount")
            pastry->applyUpgradeDiscount(value);    }
}

std::string Beverage::getType() const {
    return "Beverage";
}

double Beverage::getUpgradeCost() const {
    return 0.0;
}

void Beverage::doSave(std::ostream& os) const {
    os << "type: Beverage\n";
    os << "name: " << name << "\n";
    os << "multiplier: " << multiplier << "\n";
    os << "unlockCost: " << unlockCost << "\n";
    os << "useCost: " << useCost << "\n";
    os << "level: " << level << "\n";
    os << "effectsCount: " << effects.size() << "\n";
    os << "effects: ";

    for (const auto& [type, value] : effects)
        os << type << " " << value << " ";

    os << "\n";
    os << "target: " << targetName << "\n";
}

void Beverage::doLoad(std::istream& is) {
    std::string line;
    std::string key;
    std::string value;

    auto getKV = [&](std::string& k, std::string& v) {
        if (!std::getline(is, line)) return false;
        if (line.empty()) return false;
        const size_t pos = line.find(':');
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
        else if (key == "effects") {
            effects.clear();
            std::istringstream ev(value);
            std::string eType;
            double eValue;
            while (ev >> eType >> eValue)
                effects.emplace_back(eType, eValue);
        }
        else if (key == "target") targetName = value;
        else {
        }
    }
}