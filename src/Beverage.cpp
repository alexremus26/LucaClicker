#include "Beverage.h"
#include <iostream>
#include <sstream>

#include "ItemFactory.h"
#include "GameExceptions.h"

void Beverage::registerItem() {
    ItemFactory::getInstance().registerType(
        "Beverage",
        [](const std::map<std::string, std::string>& config) -> std::unique_ptr<Item> {

            auto require = [&](const char* key) -> const std::string& {
                const auto it = config.find(key);
                if (it == config.end()) {
                    throw InvalidFormatException(std::string("Beverage missing key: '") + key + "'");
                }
                return it->second;
            };

            auto getOr = [&](const char* key, const std::string& def) -> std::string {
                const auto it = config.find(key);
                return (it == config.end()) ? def : it->second;
            };

            try {
                const std::string& name = require("name");
                const double multiplier = std::stod(require("multiplier"));
                const double unlockCost = std::stod(require("unlockCost"));

                std::vector<std::tuple<std::string, double>> effects;
                {
                    const std::string effectsStr = getOr("effects", "");
                    std::stringstream ss(effectsStr);
                    std::string effectType;
                    double effectValue;
                    while (ss >> effectType >> effectValue) {
                        effects.emplace_back(effectType, effectValue);
                    }
                }

                const std::string targetName = getOr("target", "all");

                return std::make_unique<Beverage>(name, multiplier, unlockCost, effects, targetName);
            } catch (const std::exception& e) {
                throw InvalidFormatException(std::string("Beverage parse error: ") + e.what());
            }
        }
    );
}

Beverage::Beverage(std::string name_, const double multiplier_, const double unlockCost_,
                   std::vector<std::tuple<std::string, double>> effects_,
                   std::string targetName_)
    : Item(std::move(name_), multiplier_, unlockCost_),
      effects(std::move(effects_)),
      targetName(std::move(targetName_)) {
    useCost = unlockCost / 2;
}

Beverage::Beverage(const Beverage& other)
    : Item(other),
      effects(other.effects),
      targetName(other.targetName),
      activeBuffs(other.activeBuffs) {}

Beverage::~Beverage() {
    std::cout << "Beverage " << name << " destroyed\n";
}

Item* Beverage::clone() const {
    return new Beverage(*this);
}

double Beverage::doSellPayout() const { return 0.0; }
double Beverage::doDeliveryPayout() const { return 0.0; }

void Beverage::doPrint(std::ostream& os) const {
    os << "Use Cost: " << useCost
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

void Beverage::doUse(std::vector<std::unique_ptr<Item>>& allItems,
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

    (void)eventMessages;
    (void)eventMutex;
}

void Beverage::applyToOne(Item& item) const {
    for (const auto& effect : effects) {
        const auto& [type, value] = effect;
        item.applyEffect(type, value);
    }
}

std::string Beverage::getType() const {
    return "Beverage";
}

double Beverage::getUpgradeCost() const {
    return 0.0;
}

void Beverage::updateBuffs(const sf::Time deltaTime) {
    for (auto it = activeBuffs.begin(); it != activeBuffs.end(); ) {
        auto& remaining = std::get<2>(*it);
        remaining -= deltaTime;
        if (remaining <= sf::Time::Zero) {
            it = activeBuffs.erase(it);
        } else {
            ++it;
        }
    }
}

double Beverage::getBuffMultiplier() {
    return 1.0;
}

std::string Beverage::getTargetName() const {
    return targetName;
}

void Beverage::update(const sf::Time time) {
    updateBuffs(time);
}

double Beverage::getSpeedMultiplier() const {
    return getBuffMultiplier();
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
    os << "activeBuffsCount: " << activeBuffs.size() << "\n";
    for (const auto& buff : activeBuffs) {
        os << "activeBuff: " << std::get<0>(buff) << ", "
           << std::get<1>(buff).asSeconds() << ", "
           << std::get<2>(buff).asSeconds() << "\n";
    }
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

    for (int i = 0; i < 9 && getKV(key, value); ++i) {
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
        else if (key == "activeBuffsCount") {
            std::size_t buffsCount = std::stoul(value);
            for (std::size_t j = 0; j < buffsCount; ++j) {
                std::string buffLine;
                if (std::getline(is, buffLine)) {
                    size_t pos = buffLine.find(':');
                    if (pos != std::string::npos) {
                        std::string buffKey;
                        std::string buffValue;
                        buffKey = buffLine.substr(0, pos);
                        buffValue = buffLine.substr(pos + 2);
                        if (buffKey == "activeBuff") {
                            std::stringstream ss(buffValue);
                            double mult, dur, rem;
                            char comma;
                            ss >> mult >> comma >> dur >> comma >> rem;
                            activeBuffs.emplace_back(mult, sf::seconds(static_cast<float>(dur)), sf::seconds(static_cast<float>(rem)));
                        }
                    }
                }
            }
        }
    }
}
