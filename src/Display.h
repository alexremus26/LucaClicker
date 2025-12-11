#ifndef DISPLAY_H
#define DISPLAY_H

#include <SFML/Graphics.hpp>
#include <string>
#include "GameManager.h"
#include "Player.h"

class Display {
private:
    GameManager& gameManager;
    Player& player;

    sf::RenderWindow window;
    sf::Font font;

    int selectedIndex = 0;
    char lastAction = ' ';

    std::string warningMessage;
    sf::Clock warningClock;

    void handleUnlock(std::size_t index);

public:
    explicit Display(GameManager& gm, Player& p);
    Display(const Display& other);
    Display& operator=(const Display& other);
    ~Display();

    friend std::ostream& operator<<(std::ostream& os, const Display& d);

    void run();
};

#endif
