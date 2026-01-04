#include "Pastry.h"
#include <iostream>
#include <SFML/System/Time.hpp>

#include "ItemFactory.h"
#include "GameExceptions.h"

void Pastry::registerItem() {
    ItemFactory::getInstance().registerType(
        "Pastry",
        [](const std::map<std::string, std::string>& config) -> std::unique_ptr<Item> {

            auto require = [&](const char* key) -> const std::string& {
                const auto it = config.find(key);
                if (it == config.end()) {
                    throw InvalidFormatException(std::string("Pastry missing key: '") + key + "'");
                }
                return it->second;
            };

            try {
                const std::string& name = require("name");
                const double multiplier = std::stod(require("multiplier"));
                const double unlockCost = std::stod(require("unlockCost"));
                const double baseIncome = std::stod(require("baseIncome"));
                const double upgradeCost = std::stod(require("upgradeCost"));
                const sf::Time duration = sf::seconds(std::stof(require("duration")));

                return std::make_unique<Pastry>(name, multiplier, unlockCost, baseIncome, upgradeCost, duration);
            } catch (const std::exception& e) {
                throw InvalidFormatException(std::string("Pastry parse error: ") + e.what());
            }
        }
    );
}

Pastry::Pastry(std::string name_, const double multiplier_, const double unlockCost_,
               const double baseIncome_, const double upgradeCost_, const sf::Time duration_)
    : Item(std::move(name_), multiplier_, unlockCost_),
      baseIncome(baseIncome_),
      upgradeCost(upgradeCost_),
      duration(duration_) {
}

Pastry::Pastry(const Pastry &other) = default;

Pastry::~Pastry() = default;

Item *Pastry::clone() const {
    return new Pastry(*this);
}

double Pastry::doSellPayout() const {
    return baseIncome;
}

double Pastry::doDeliveryPayout() const {
    return baseIncome;
}

void Pastry::doPrint(std::ostream &os) const {
    os  << "  Income: " << baseIncome
        << " (x" << multiplier << " = " << baseIncome * multiplier<< ")"
        << " | Upgrade Cost: " << upgradeCost
        << " | Level: " << level
        << " | Multiplier: " << multiplier;
}

void Pastry::doUpgrade() {
    level++;
    baseIncome *= multiplier;
    upgradeCost *= multiplier;

    if (level == 10 || level == 25 || level == 50 || level == 100) {
        baseIncome *= 2.0;
    }
}

sf::Time Pastry::doComputeDuration() const {
    return sf::seconds(2.0f + static_cast<float>(unlockCost) / 100.0f);
}

void Pastry::applyUpgradeDiscount(const double factor) {
    upgradeCost *= factor;
}

double Pastry::getUpgradeCost() const {
    return upgradeCost;
}

bool Pastry::isUsable() const {
    return false;
}

double Pastry::doComputeIncomePerSecond() const {
    return baseIncome/static_cast<double>(duration.asSeconds());
}

void Pastry::applyEffect(const std::string& type, const double value) {
    if (type == "profit_multiplier") {
        baseIncome *= value;
    } else if (type == "upgrade_discount") {
        applyUpgradeDiscount(value);
    }
}

std::string Pastry::doGetEffectDescription() const {
    return "Generates " + std::to_string(static_cast<int>(doSellPayout())) + " RON";
}

void Pastry::doUse(std::vector<std::unique_ptr<Item>>&, std::queue<std::string>&, std::mutex&) {
}

std::string Pastry::getType() const {
    return "Pastry";
}

void Pastry::doDrawRaffle(Player &, double , std::queue<std::string> &,
    std::mutex &) const {
}

void Pastry::doSave(std::ostream& os) const {
    os << "type: Pastry\n";
    os << "name: " << name << "\n";
    os << "multiplier: " << multiplier << "\n";
    os << "unlockCost: " << unlockCost << "\n";
    os << "useCost: " << useCost << "\n";
    os << "level: " << level << "\n";
    os << "baseIncome: " << baseIncome << "\n";
    os << "upgradeCost: " << upgradeCost << "\n";
    os << "duration: " << duration.asSeconds() << "\n";
}

void Pastry::doLoad(std::istream& is) {
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

    for (int i = 0; i < 8 && getKV(key, value); ++i) {
        if (key == "name") name = value;
        else if (key == "multiplier") multiplier = std::stod(value);
        else if (key == "unlockCost") unlockCost = std::stod(value);
        else if (key == "useCost") useCost = std::stod(value);
        else if (key == "level") level = std::stoi(value);
        else if (key == "baseIncome") baseIncome = std::stod(value);
        else if (key == "upgradeCost") upgradeCost = std::stod(value);
        else if (key == "duration") duration = sf::seconds(std::stof(value));
    }
}