#include "Player.h"
#include <iostream>

Player::Player(std::string  playerName_,const double money_) : playerName(std::move(playerName_)), money(money_) {}

Player::Player(const Player& player) : playerName(player.playerName) , money(player.money){}

Player::~Player()= default;

Player &Player::operator=(const Player &player) {
    playerName = player.playerName;
    money = player.money;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const Player &player) {
    os << player.playerName << " " << player.money;
    return os;
}

const double &Player::getMoney() const { return money; }


bool Player::tryPay(const double amount) {
    if (money < amount) return false;
    money -= amount;
    return true;
}

void Player::earn(const double amount) {
    money += amount;
}

void Player::save(std::ostream& os) const {
    os << "money: " << money << "\n";
}

void Player::load(std::istream& is) {
    std::string line;
    std::getline(is, line);
    size_t pos = line.find(':');
    if (pos != std::string::npos) {
        const std::string key = line.substr(0, pos);
        const std::string value = line.substr(pos + 2);
        if (key == "money") {
            money = std::stod(value);
        }
    }
}


