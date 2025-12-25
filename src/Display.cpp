#include "Display.h"

#include <fstream>

#include "Beverage.h"
#include "Pastry.h"
#include "Sandwich.h"
#include "GameExceptions.h"
#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <SFML/Audio/Sound.hpp>

Display::Display(Game& manager)
    : gameManager(manager)
{
}

Display::~Display() = default;

std::ostream& operator<<(std::ostream& os, const Display& display)
{
    os << display.gameManager;
    return os;
}

void Display::drawProgressBar(const float progress, const float x, const float y)
{
    const float clampedProgress = std::clamp(progress, 0.f, 1.f);

    static const sf::Texture& outlineTex =
        ResourceManager::instance().getTexture(
            "../assets/textures/ProgressBarOutline.png"
        );

    static const sf::Texture& fillTex =
        ResourceManager::instance().getTexture(
            "../assets/textures/ProgressBarContent.png"
        );

    sf::Sprite outline(outlineTex);
    sf::Sprite fill(fillTex);

    outline.setPosition({x, y});
    fill.setPosition({x, y});

    const sf::Vector2u size = fillTex.getSize();
    const unsigned int clippedWidth =
        static_cast<unsigned int>(static_cast<float>(size.x) * clampedProgress);

    fill.setTextureRect({
        {3, -8},
        {static_cast<int>(clippedWidth), static_cast<int>(size.y)}
    });

    window.draw(outline);
    window.draw(fill);
}
float Display::drawItemAndReturnHeight(const Item& item,
                                      const size_t realIndex,
                                      const int displayIndex,
                                      float x,
                                      float y)
{
    sf::Text title(font);
    title.setCharacterSize(TITLE_SIZE);
    title.setFillColor(
        selectedIndex == displayIndex
        ? sf::Color(255, 220, 120)
        : sf::Color::White
    );

    if (gameManager.isUnlocked(realIndex)) {
        title.setString(
            "[" + std::to_string(displayIndex + 1) + "] " +
            item.getName() + " (" + item.getType() + ")"
        );
    } else {
        title.setString(
            "[" + std::to_string(displayIndex + 1) +
            "] LOCKED - Cost: " +
            std::to_string(static_cast<int>(item.getUnlockCost())) + " RON"
        );
    }

    sf::Text details(font);
    details.setCharacterSize(DETAIL_SIZE);
    details.setFillColor(sf::Color(200, 200, 200));
    std::ostringstream oss;
    item.print(oss);
    details.setString(oss.str());

    title.setPosition({ x, y });
    window.draw(title);

    float blockHeight = title.getGlobalBounds().size.y;

    details.setPosition({ x, y + blockHeight + 6.f });
    window.draw(details);

    blockHeight += details.getGlobalBounds().size.y + 6.f;
    const float progress = gameManager.anyProgress(realIndex);
    if (progress != 0) {
        drawProgressBar(
            progress,
            x,
            y + blockHeight + 6.f
        );
        blockHeight += PROGRESS_HEIGHT + 6.f;
    }

    return blockHeight;
}

void Display::initWindowAndFont() {

    if (!font.openFromFile("assets/font/MightySouly-lxggD.ttf"))
        throw FontLoadingException("assets/font/MightySouly-lxggD.ttf");
}

Display::MenuResult Display::menu()
{
    const std::string savePath = "../data/savefile.txt";

    initWindowAndFont();

    auto saveExists = [&]() {
        std::ifstream file(savePath);
        return file.good();
    };

    auto deleteSave = [&]() {
        std::remove(savePath.c_str());
    };

    constexpr sf::Vector2u menuSize{1200, 1100};
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    window.create(
        sf::VideoMode(menuSize, desktop.bitsPerPixel),
        "Luca Clicker",
        sf::Style::Titlebar | sf::Style::Close
    );

    sf::Sprite background(
        ResourceManager::instance().getTexture(
            "../assets/textures/MenuBackground.png"
        )
    );

    const sf::Vector2u bgSize = background.getTexture().getSize();
    background.setScale({
        static_cast<float>(menuSize.x) / bgSize.x,
        static_cast<float>(menuSize.y) / bgSize.y
    });

    sf::Sound soundtrack(
        ResourceManager::instance().getSound(
            "../assets/audio/adventure_capitalist_theme_song.wav"
        )
    );
    soundtrack.setLooping(true);
    soundtrack.setVolume(50.f);
    soundtrack.play();

    const sf::Texture& buttonTex =
        ResourceManager::instance().getTexture(
            "../assets/textures/MenuButton.png"
        );

    std::vector<sf::Sprite> buttons;
    buttons.emplace_back(buttonTex); // New Game
    buttons.emplace_back(buttonTex); // Load Game

    for (auto& b : buttons)
        b.setScale({0.75, 0.75});

    float startY  = menuSize.y * 0.4f;

    for (std::size_t i = 0; i < buttons.size(); ++i) {
        float spacing = 100.f;
        const sf::FloatRect bounds = buttons[i].getGlobalBounds();
        buttons[i].setPosition({
            (menuSize.x - bounds.size.x) * 0.5f,
            startY + i * (bounds.size.y + spacing)
        });
    }

    std::vector<sf::Text> labels;
    labels.emplace_back(font, "New Game", 50);
    labels.emplace_back(font, "Saved Game", 50);

    for (std::size_t i = 0; i < labels.size(); ++i) {
        labels[i].setFillColor(sf::Color(200, 200, 200));

        const sf::FloatRect tb = labels[i].getLocalBounds();
        labels[i].setOrigin({
            tb.position.x + tb.size.x * 0.5f,
            tb.position.y + tb.size.y * 0.5f
        });

        const sf::FloatRect bb = buttons[i].getGlobalBounds();
        labels[i].setPosition({
            bb.position.x + bb.size.x * 0.5f,
            bb.position.y + bb.size.y * 0.5f
        });
    }

    sf::Text title(font, "Luca Clicker", 160);
    title.setFillColor(sf::Color(200, 200, 200));

    const sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin({
        tb.position.x + tb.size.x * 0.5f,
        tb.position.y + tb.size.y * 0.5f
    });

    title.setPosition({
        menuSize.x * 0.5f,
        menuSize.y * 0.2f
    });

    sf::Text warning(font, "", 32);
    warning.setFillColor(sf::Color::Red);
    warning.setPosition({
        menuSize.x * 0.5f,
        menuSize.y * 0.3f
    });

    while (window.isOpen()) {

        const sf::Vector2f mouse =
            window.mapPixelToCoords(sf::Mouse::getPosition(window));

        while (const auto event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
                return MenuResult::Exit;
            }

            if (event->is<sf::Event::MouseButtonPressed>()) {
                const auto* me = event->getIf<sf::Event::MouseButtonPressed>();

                if (me->button == sf::Mouse::Button::Left) {
                    for (std::size_t i = 0; i < buttons.size(); ++i) {
                        if (buttons[i].getGlobalBounds().contains(mouse)) {

                            if (i == 0) {
                                if (saveExists())
                                    deleteSave();
                                window.close();
                                return MenuResult::NewGame;
                            }

                            if (i == 1) {
                                if (!saveExists()) {
                                    warning.setString("No saved game found!");
                                    warningClock.restart();
                                    break;
                                }
                                window.close();
                                return MenuResult::LoadGame;
                            }
                        }
                    }
                }
            }
        }

        for (auto& b : buttons) {
            if (b.getGlobalBounds().contains(mouse))
                b.setColor(sf::Color(255, 215, 0));
            else
                b.setColor(sf::Color::White);
        }
        window.clear();
        window.draw(background);
        window.draw(title);

        if (!warning.getString().isEmpty() &&
            warningClock.getElapsedTime().asSeconds() < 2.5f)
            window.draw(warning);

        for (const auto& b : buttons)
            window.draw(b);

        for (const auto& t : labels)
            window.draw(t);

        window.display();
    }

    return MenuResult::Exit;
}

void Display::run()
{
    if (menu() == MenuResult::Exit)
        return;

    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(
        sf::VideoMode({desktop.size.x, desktop.size.y}, desktop.bitsPerPixel),
        "Luca Clicker",
        sf::Style::Default
    );
    window.setView(window.getDefaultView());

    const float winW = static_cast<float>(window.getSize().x);
    const float winH = static_cast<float>(window.getSize().y);

    sf::Text moneyText(font, "", 42);
    moneyText.setFillColor(sf::Color(255, 220, 120));
    moneyText.setPosition({winW * 0.03f, winH * 0.03f});

    sf::Sprite background(
        ResourceManager::instance().getTexture(
            "../assets/textures/GameBackground.png"
        )
    );
    background.setScale({
        winW / background.getTexture().getSize().x,
        winH / background.getTexture().getSize().y
    });

    const sf::Texture& holderTex =
        ResourceManager::instance().getTexture("../assets/textures/ItemHolder.png");
    const sf::Texture& buyTex =
        ResourceManager::instance().getTexture("../assets/textures/BuyButton.png");
    const sf::Texture& timeTex =
        ResourceManager::instance().getTexture("../assets/textures/TimeIntervalButton.png");

    std::vector<sf::Sprite> holders, buyButtons, upgradeButtons;
    std::vector<sf::Text> buyTexts, upgradeTexts;

    std::vector<bool> buyHovered(5, false);
    std::vector<bool> upgradeHovered(5, false);

    float baseScale = winH * 0.00025f;
    float buyScale = baseScale * 1.25f;
    float timeScale = baseScale * 0.85f;

    float leftX = winW * 0.04f;
    float startY = winH * 0.18f;
    float spacing = winH * 0.155f;
    float progressX = leftX + holderTex.getSize().x * baseScale + winW * 0.015f;
    float buyX = progressX;
    float timeX = buyX + buyTex.getSize().x * buyScale + winW * 0.01f;

    for (int i = 0; i < 5; ++i) {
        float y = startY + i * spacing;

        sf::Sprite h(holderTex);
        h.setScale({baseScale, baseScale});
        h.setPosition({leftX, y});
        holders.push_back(h);

        sf::Sprite b(buyTex);
        b.setScale({buyScale, buyScale});
        b.setPosition({buyX, y + winH * 0.055f});
        buyButtons.push_back(b);

        sf::Text bt(font, "BUY", 32);
        bt.setFillColor(sf::Color::Black);
        bt.setOrigin(bt.getLocalBounds().getCenter());
        bt.setPosition(b.getGlobalBounds().getCenter());
        buyTexts.push_back(bt);

        sf::Sprite t(timeTex);
        t.setScale({timeScale, timeScale});
        t.setPosition({timeX, y + winH * 0.062f});
        upgradeButtons.push_back(t);

        sf::Text tt(font, "Upgrade", 26);
        tt.setFillColor(sf::Color::White);
        tt.setOrigin(tt.getLocalBounds().getCenter());
        tt.setPosition(t.getGlobalBounds().getCenter());
        upgradeTexts.push_back(tt);
    }

    while (window.isOpen()) {
        gameManager.updateSelling();

        while (auto e = window.pollEvent()) {
            if (e->is<sf::Event::Closed>()) {
                window.close();
            }

            if (e->is<sf::Event::MouseButtonPressed>()) {
                if (e->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mouse =
                        window.mapPixelToCoords(sf::Mouse::getPosition(window));

                    for (int i = 0; i < 5; ++i) {
                        if (buyButtons[i].getGlobalBounds().contains(mouse)) {
                            if (!gameManager.isUnlocked(i)) {
                                gameManager.unlockItem(i);
                            } else {
                                auto& item = *gameManager.getItems()[i];
                                gameManager.runSellingLoop(item, i);
                            }
                        }

                        if (upgradeButtons[i].getGlobalBounds().contains(mouse)) {
                            if (gameManager.isUnlocked(i)) {
                                auto& item = *gameManager.getItems()[i];
                                gameManager.upgrade(item);
                            }
                        }
                    }
                }
            }
        }

        window.clear();
        window.draw(background);

        sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        for (int i = 0; i < 5; ++i) {
            buyHovered[i] = buyButtons[i].getGlobalBounds().contains(mouse);
            upgradeHovered[i] = upgradeButtons[i].getGlobalBounds().contains(mouse);
        }

        double money = gameManager.getPlayerMoney();
        moneyText.setString("$ " + std::to_string(static_cast<long long>(money)));
        window.draw(moneyText);

        for (int i = 0; i < 5; ++i) {
            window.draw(holders[i]);


            buyButtons[i].setColor(buyHovered[i] ? sf::Color(255, 230, 160) : sf::Color::White);
            upgradeButtons[i].setColor(upgradeHovered[i] ? sf::Color(200, 200, 255) : sf::Color::White);

            window.draw(buyButtons[i]);
            window.draw(buyTexts[i]);
            window.draw(upgradeButtons[i]);
            window.draw(upgradeTexts[i]);
            drawProgressBar(gameManager.anyProgress(i), progressX, startY + i * spacing);
        }

        window.display();
    }
}