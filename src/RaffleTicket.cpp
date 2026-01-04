#include "RaffleTicket.h"
#include "ItemFactory.h"
#include "GameExceptions.h"
#include "Player.h"
#include <iostream>
#include <sstream>

void RaffleTicket::registerItem() {
    ItemFactory::getInstance().registerType(
        "RaffleTicket",
        [](const std::map<std::string, std::string>& config) -> std::unique_ptr<Item> {
            auto require = [&](const char* key) -> const std::string& {
                const auto it = config.find(key);
                if (it == config.end()) {
                    throw InvalidFormatException(std::string("RaffleTicket missing key: '") + key + "'");
                }
                return it->second;
            };

            try {
                const std::string& name = require("name");
                const double unlockCost = std::stod(require("unlockCost"));
                const sf::Time minDuration = sf::seconds(std::stof(require("minDuration")));
                const sf::Time maxDuration = sf::seconds(std::stof(require("maxDuration")));
                return std::make_unique<RaffleTicket>(name, unlockCost, minDuration, maxDuration);
            } catch (const std::exception& e) {
                throw InvalidFormatException(std::string("RaffleTicket parse error: ") + e.what());
            }
        }
    );
}

RaffleTicket::RaffleTicket(std::string name, const double unlockCost, const sf::Time minDuration, const sf::Time maxDuration)
    : Item(std::move(name), 1.0, unlockCost), minDuration(minDuration), maxDuration(maxDuration), rng(std::random_device{}()) {
    useCost = unlockCost;
}

RaffleTicket::RaffleTicket(const RaffleTicket& other)
    : Item(other), minDuration(other.minDuration), maxDuration(other.maxDuration), rng(std::random_device{}()) {}

Item* RaffleTicket::clone() const {
    return new RaffleTicket(*this);
}

double RaffleTicket::doSellPayout() const { return 0.0; }
double RaffleTicket::doDeliveryPayout() const { return 0.0; }

void RaffleTicket::doPrint(std::ostream& os) const {
    os << "Use Cost: " << useCost
       << " | Level: " << level
       << " | Time Warp: " << minDuration.asSeconds() << "s - " << maxDuration.asSeconds() << "s";
}

void RaffleTicket::doUpgrade() {
    level++;
    minDuration += sf::seconds(1.0f * static_cast<float>(level));
    maxDuration += sf::seconds(2.0f * static_cast<float>(level));
    useCost *= 1.2;
}

sf::Time RaffleTicket::doComputeDuration() const {
    std::uniform_real_distribution<float> dist(minDuration.asSeconds(), maxDuration.asSeconds());
    return sf::seconds(dist(rng));
}

std::string RaffleTicket::doGetEffectDescription() const {
    std::ostringstream oss;
    oss << "Instantly gain between " << minDuration.asSeconds() << " and " << maxDuration.asSeconds() << " seconds of income.";
    return oss.str();
}

void RaffleTicket::doSave(std::ostream& os) const {
    os << "type:RaffleTicket\n";
    os << "name: " << name << "\n";
    os << "multiplier: " << multiplier << "\n";
    os << "unlockCost: " << unlockCost << "\n";
    os << "useCost: " << useCost << "\n";
    os << "level: " << level << "\n";
    os << "minDuration: " << minDuration.asSeconds() << "\n";
    os << "maxDuration: " << maxDuration.asSeconds() << "\n";
}

void RaffleTicket::doLoad(std::istream& is) {
    std::string line, key, value;
    auto getKV = [&](std::string& k, std::string& v) {
        if (!std::getline(is, line) || line.empty()) return false;
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
        else if (key == "minDuration") minDuration = sf::seconds(std::stof(value));
        else if (key == "maxDuration") maxDuration = sf::seconds(std::stof(value));
    }
}

std::string RaffleTicket::getType() const {
    return "RaffleTicket";
}

void RaffleTicket::doUse(std::vector<std::unique_ptr<Item>>&, std::queue<std::string>&, std::mutex&) {
}

double RaffleTicket::getUpgradeCost() const {
    return useCost * 1.5;
}

bool RaffleTicket::isUsable() const {
    return true;
}

double RaffleTicket::doComputeIncomePerSecond() const {
    return 0.0;
}

void RaffleTicket::doDrawRaffle(Player& player, const double incomePerSecond,
                                  std::queue<std::string>& eventMessages,
                                  std::mutex& eventMutex) const {
    const sf::Time warpDuration = getDuration();
    const double payout = incomePerSecond * warpDuration.asSeconds();

    player.earn(payout);

    std::ostringstream oss;
    oss.precision(2);
    oss << "Raffle Ticket time warp triggered! You gained " << std::fixed << payout << " money from " << warpDuration.asSeconds() / 60 << "minutes of production!";

    const std::lock_guard lock(eventMutex);
    eventMessages.push(oss.str());
}
