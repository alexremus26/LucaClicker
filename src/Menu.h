#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>

class Menu {
private:
    sf::RenderWindow& window;
    sf::Font font;

    std::optional<sf::Sprite> background;
    std::optional<sf::Text> title;
    std::optional<sf::Text> warning;

    sf::Clock warningClock;

    std::vector<sf::Sprite> buttons;
    std::vector<sf::Text> labels;

    enum class Result { NewGame, LoadGame, Exit };

    Result result = Result::Exit;

    void init();
    void loadFont();

    void buildBackground();
    void buildButtons();
    void buildLabels();
    void buildTitle();
    void buildWarning();

    void pollEvents();
    void update();
    void draw() const;

    static bool saveExists() ;
    static void deleteSave() ;

    static constexpr sf::Vector2u MenuSize{1200u, 1100u};
    static constexpr auto SavePath = "data/savefile.txt";

public:

    explicit Menu(sf::RenderWindow& window);
    void show();

    [[nodiscard]] bool wantsLoadGame() const;
    [[nodiscard]] bool wantsExit() const;
};

#endif // MENU_H