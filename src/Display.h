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

    int  selectedIndex = 0;
    char lastAction    = ' ';

    std::string warningMessage;
    sf::Clock   warningClock;

    static constexpr float LEFT_MARGIN   = 40.f;
    static constexpr float TOP_MARGIN    = 20.f;
    static constexpr float ITEM_SPACING  = 40.f;

    static constexpr unsigned HEADER_SIZE = 40;
    static constexpr unsigned TITLE_SIZE  = 50;
    static constexpr unsigned DETAIL_SIZE = 30;

    static constexpr float PROGRESS_WIDTH  = 600.f;
    static constexpr float PROGRESS_HEIGHT = 20.f;

    static constexpr float WARNING_Y_OFFSET = 80.f;

    void handleUnlock(std::size_t index);
    void drawProgressBar(float progress, float x, float y);

public:
    explicit Display(GameManager& gm, Player& p);
    Display(const Display& other);
    Display& operator=(const Display& other);
    ~Display();

    friend std::ostream& operator<<(std::ostream& os, const Display& d);

    void run();
};

#endif // DISPLAY_H
