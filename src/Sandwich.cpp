#include "Sandwich.h"
#include <ostream>
#include <algorithm>
#include <SFML/System/Time.hpp>

#include "ItemFactory.h"
#include "GameExceptions.h"

void Sandwich::registerItem() {
    ItemFactory::getInstance().registerType(
        "Sandwich",
        [](const std::map<std::string, std::string>& config) -> std::unique_ptr<Item> {

            auto require = [&](const char* key) -> const std::string& {
                const auto it = config.find(key);
                if (it == config.end()) {
                    throw InvalidFormatException(std::string("Sandwich missing key: '") + key + "'");
                }
                return it->second;
            };

            try {
                const std::string& name = require("name");
                const double unlockCost = std::stod(require("unlockCost"));
                const double fastMult = std::stod(require("fastMultiplier"));
                const double slowMult = std::stod(require("slowMultiplier"));
                const double fastChance = std::stod(require("fastChance"));
                const sf::Time baseDuration = sf::seconds(std::stof(require("baseDuration")));

                return std::make_unique<Sandwich>(name, unlockCost, fastMult, slowMult, fastChance, baseDuration);
            } catch (const std::exception& e) {
                throw InvalidFormatException(std::string("Sandwich parse error: ") + e.what());
            }
        }
    );
}

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

double Sandwich::doSellPayout() const { return 0.0; }
double Sandwich::doDeliveryPayout() const { return 0.0; }

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

    this->fastChance = std::min(1.0, this->fastChance + 0.01);
    this->fastMultiplier += 0.05;
    if (this->slowMultiplier > 0.1)
        this->slowMultiplier -= 0.01;
}

sf::Time Sandwich::doComputeDuration() const {
    return sf::seconds(0.f);
}

std::string Sandwich::doGetEffectDescription() const {
    return "Random speed: fast x" + std::to_string(fastMultiplier) +
           " or slow x" + std::to_string(slowMultiplier);
}

void Sandwich::doUse(std::vector<std::unique_ptr<Item>>& allItems,
                     std::queue<std::string>& eventMessages,
                     std::mutex& eventMutex) {

    (void)allItems;

    multiplier = rollFastEffect() ? fastMultiplier : slowMultiplier;

    const sf::Time duration = getDuration();

    const std::lock_guard lock(eventMutex);

    if (currentSpeedBuff.has_value()) {
        std::get<0>(*currentSpeedBuff) = multiplier;
        std::get<1>(*currentSpeedBuff) = duration;
        std::get<2>(*currentSpeedBuff) = duration;
        eventMessages.push("Refreshed " + name + "! Speed x" + std::to_string(multiplier) + " for " + std::to_string(duration.asSeconds()) + "s.");
    } else {
        currentSpeedBuff = std::make_tuple(multiplier, duration, duration);
        eventMessages.push("Used " + name + "! Speed x" + std::to_string(multiplier) + " for " + std::to_string(duration.asSeconds()) + "s.");
    }
}

std::string Sandwich::getType() const {
    return "Sandwich";
}

void Sandwich::doDrawRaffle(Player &, double , std::queue<std::string> &,
    std::mutex &) const {
}

double Sandwich::getUpgradeCost() const {
    return 0.0;
}

void Sandwich::update(const sf::Time time) {
    if (currentSpeedBuff.has_value()) {
        std::get<1>(*currentSpeedBuff) -= time;
        if (std::get<1>(*currentSpeedBuff) <= sf::Time::Zero) {
            currentSpeedBuff.reset();
        }
    }
}

double Sandwich::getSpeedMultiplier() const {
    if (currentSpeedBuff.has_value()) {
        return std::get<0>(*currentSpeedBuff);
    }
    return 1.0;
}

double Sandwich::doComputeIncomePerSecond() const {
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

    if (currentSpeedBuff.has_value()) {
        os << "hasBuff: 1\n";
        os << "buffMultiplier: " << std::get<0>(*currentSpeedBuff) << "\n";
        os << "buffRemaining: " << std::get<1>(*currentSpeedBuff).asSeconds() << "\n";
        os << "buffTotal: " << std::get<2>(*currentSpeedBuff).asSeconds() << "\n";
    } else {
        os << "hasBuff: 0\n";
    }
}

void Sandwich::doLoad(std::istream& is) {
    std::string line, key, value;

    auto getKV = [&](std::string& k, std::string& v) {
        if (!std::getline(is, line)) return false;
        if (line.empty()) return false;
        const size_t pos = line.find(':');
        if (pos == std::string::npos) return false;
        k = line.substr(0, pos);
        v = line.substr(pos + 2);
        return true;
    };

    for (int i = 0; i < 14 && getKV(key, value); ++i) {
        if (key == "name") name = value;
        else if (key == "multiplier") multiplier = std::stod(value);
        else if (key == "unlockCost") unlockCost = std::stod(value);
        else if (key == "useCost") useCost = std::stod(value);
        else if (key == "level") level = std::stoi(value);
        else if (key == "fastMultiplier") fastMultiplier = std::stod(value);
        else if (key == "slowMultiplier") slowMultiplier = std::stod(value);
        else if (key == "fastChance") fastChance = std::stod(value);
        else if (key == "baseDuration") baseDuration = sf::seconds(std::stof(value));
        else if (key == "hasBuff") {
            if (value == "1") {
                std::string k2, v2;
                getKV(k2, v2);
                double mult = std::stod(v2);
                getKV(k2, v2);
                const double rem  = std::stod(v2);
                getKV(k2, v2);
                const double tot  = std::stod(v2);
                currentSpeedBuff = std::make_tuple(
                    mult,
                    sf::seconds(static_cast<float>(rem)),
                    sf::seconds(static_cast<float>(tot))
                );
            } else {
                currentSpeedBuff.reset();
            }
        }
    }
}

