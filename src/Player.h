#ifndef OOP_PLAYER_H
#define OOP_PLAYER_H

#include <string>

class Player {
    std::string playerName;
    double money;

public:
    Player(std::string  playerName_,double money_);
    Player(const Player& player);
    ~Player();
    Player& operator=(const Player& player);
    friend std::ostream& operator<<(std::ostream& os, const Player& player);

    [[nodiscard]] const double& getMoney() const;
    void setMoney(double money_);
    bool tryPay(double amount);
    void earn(double amount);
    [[nodiscard]] bool enoughMoney(double amount) const;
};



#endif //OOP_PLAYER_H