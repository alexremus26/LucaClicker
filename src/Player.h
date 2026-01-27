#ifndef OOP_PLAYER_H
#define OOP_PLAYER_H

#include <string>

class Player {
    std::string playerName;
    double money;
    bool m_hasWon = false;

public:
    Player(std::string  playerName_,double money_);
    Player(const Player& player);
    ~Player();
    Player& operator=(const Player& player);
    friend std::ostream& operator<<(std::ostream& os, const Player& player);

    [[nodiscard]] const double& getMoney() const;
    [[nodiscard]] bool hasWon() const;

    bool tryPay(double amount);
    void earn(double amount);

    void save(std::ostream& os) const;
    void load(std::istream& is);
};



#endif //OOP_PLAYER_H