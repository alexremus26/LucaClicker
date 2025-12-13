#include "Player.h"
#include <iostream>

Player::Player(std::string  playerName_,const double money_) : playerName(std::move(playerName_)), money(money_) {}

Player::Player(const Player& player) : playerName(player.playerName) , money(player.money){}

Player::~Player(){std::cout<<"Player-ul "<< playerName <<" a fost distrus! \n ";}

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
void Player::setMoney(double const money_) { money = money_; }

bool Player::tryPay(const double amount) {
    if (money < amount) return false;
    money -= amount;
    return true;
}

void Player::earn(const double amount) {
    money += amount;
}

bool Player::enoughMoney(const double amount) const {
    if (money >= amount) {
        return true;
    }
    else return false;
}

double Player::moneyForUI() const {
    return money;
}

