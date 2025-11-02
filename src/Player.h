#ifndef OOP_PLAYER_H
#define OOP_PLAYER_H

#include <string>

class Player {
    std::string playerName;
    double money;

public:
    Player(std::string  playerName_,const double money_);
    Player(const Player& player);
    ~Player();
    Player& operator=(const Player& player);
    friend std::ostream& operator<<(std::ostream& os, const Player& player);

    const double& getMoney() const;
    void setMoney(double const money_);
};


#endif //OOP_PLAYER_H