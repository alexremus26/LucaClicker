#ifndef OOP_DISPLAY_H
#define OOP_DISPLAY_H
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "GameManager.h"

class Display {

    sf::RenderWindow window;
    sf::Font font;

    GameManager& gameManager;
    Player& player;

    std::vector<bool> unlocked;
    bool running = true;
    char lastAction = ' ';
    int selectedIndex = 1;
    std::string warningMessage;
    sf::Clock warningClock;

public:
    Display(GameManager& gm, Player& p);
    Display(const Display& other);
    ~Display();
    Display& operator=(const Display& other);
    friend std::ostream& operator<<(std::ostream& os, const Display& d);
    void run();
};

#endif //OOP_DISPLAY_H