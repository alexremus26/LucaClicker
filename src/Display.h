#ifndef DISPLAY_H
#define DISPLAY_H

#include <SFML/Graphics.hpp>
#include <string>
#include <optional>

#include "Game.h"

class Display {
private:
    Game& gameManager;

    sf::RenderWindow window;
    std::optional<sf::RenderTexture> renderTexture;
    sf::Font font;

    int selectedIndex = -1;
    std::string warningMessage;
    sf::Clock warningClock;

    static constexpr sf::Vector2u menuSize{1200, 1100};
    static constexpr float referenceHeight = 1080.f;

    void drawProgressBar(float progress, float x, float y, float scale);
    void initWindowAndFont();

    enum class MenuResult {
        NewGame,
        LoadGame,
        Exit
    };

    MenuResult menu();

public:
    explicit Display(Game& manager);
    Display(const Display& other) = delete;
    Display& operator=(const Display& other) = delete;
    ~Display();
    friend std::ostream& operator<<(std::ostream& os, const Display& display);

    void run();
};

#endif // DISPLAY_H