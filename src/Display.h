#ifndef DISPLAY_H
#define DISPLAY_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio/Sound.hpp>

#include <optional>
#include <string>
#include <vector>

#include "Game.h"

class Display {
private:
    static constexpr sf::Vector2u MenuSize{1200u, 1100u};
    static constexpr float ReferenceHeight = 1080.f;
    static constexpr int ItemCount = 5;

    struct RowUI {
        sf::Sprite holder;
        sf::Sprite icon;

        sf::Sprite primaryButton;
        sf::Sprite secondaryButton;

        sf::Text name;
        sf::Text level;

        sf::Text primaryLabel;
        sf::Text primaryValue;

        sf::Text secondaryLabel;
        sf::Text secondaryValue;

        bool primaryHovered = false;
        bool secondaryHovered = false;
        bool holderHovered = false;
    };

    struct TooltipUI {
        sf::RectangleShape background;
        std::optional<sf::Text> text;
        bool visible = false;
        std::string content;
    };

    struct GameUI {
        std::optional<sf::Text> moneyText;
        std::vector<RowUI> items;
        std::vector<RowUI> beverages;
        TooltipUI tooltip;
    };

    struct AudioState {
        std::optional<sf::Sound> soundtrack;
        bool started = false;
    };

    Game& gameManager;
    sf::RenderWindow window;
    sf::Font font;

    AudioState audio;
    GameUI ui;

    float heightScale = 1.f;
    float baseScale = 1.f;
    float buyScale = 1.f;
    float timeScale = 1.f;

    float leftX = 0.f;
    float startY = 0.f;
    float spacingY = 0.f;

    float buyX = 0.f;
    float timeX = 0.f;

    float secondColumnX = 0.f;
    float useButtonX = 0.f;

    float progressX = 0.f;
    float progressScale = 1.f;

    void loadFont();
    void createMenuWindow();
    void createGameWindow();

    void initGame();
    void computeLayout();
    void initAudio();
    void initMoneyText();
    void buildRows();
    RowUI buildItemRow(int index) const;
    RowUI buildBeverageRow(int index) const;

    sf::Text makeSmallLabel(const std::string& str, const sf::FloatRect& bounds, const sf::Color& color) const;
    sf::Text makeValueLabel(const std::string& str, const sf::FloatRect& bounds, const sf::Color& color) const;
    sf::Text makeCenteredText(const std::string& str, unsigned int size, const sf::Color& color, const sf::Vector2f& center) const;

    void gameLoop();
    void pollEvents();
    void onClick(const sf::Vector2f& mouse) const;

    void updateFrame();
    void updateHover(const sf::Vector2f& mouse);
    void updateMoney();
    void updateRowsText();

    void updateTooltip(const sf::Vector2f& mouse);
    std::string tooltipContent() const;

    void renderFrame();
    void drawBackground();
    void drawMoney();
    void drawItems();
    void drawBeverages();
    void drawTooltip();
    void drawProgressBar(float progress, float x, float y, float scale);

    void handleItemClick(int index, const sf::Vector2f& mouse) const;
    void handleBeverageClick(int index, const sf::Vector2f& mouse) const;

    static sf::Vector2f rectCenter(const sf::FloatRect& r);
    static sf::Texture& getEmptyTexture();

public:
    explicit Display(Game& manager);
    Display(const Display&) = delete;
    Display& operator=(const Display&) = delete;
    ~Display();

    friend std::ostream& operator<<(std::ostream& os, const Display& display);

    void run();
};

#endif
