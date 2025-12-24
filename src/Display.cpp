#include "Display.h"

#include <fstream>

#include "Beverage.h"
#include "Pastry.h"
#include "Sandwich.h"


#include <iostream>
#include <sstream>

#include "GameExceptions.h"
#include "ResourceManager.h"
#include "SFML/Audio/Sound.hpp"

Display::Display(Game& manager)
    : gameManager(manager)
{
    initWindowAndFont();
}

Display::~Display() = default;

std::ostream& operator<<(std::ostream& os, const Display& display)
{
    os << display.gameManager;
    return os;
}

void Display::drawProgressBar(const float progress, const float x, const float y)
{
    sf::RectangleShape back({ PROGRESS_WIDTH, PROGRESS_HEIGHT });
    back.setFillColor(sf::Color(70, 70, 70));
    back.setPosition({ x, y });

    sf::RectangleShape fill({
        PROGRESS_WIDTH * progress,
        PROGRESS_HEIGHT
    });
    fill.setFillColor(sf::Color(60, 200, 90));
    fill.setPosition({ x, y });

    window.draw(back);
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
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    window.create(
        sf::VideoMode({ desktop.size.x, desktop.size.y }, desktop.bitsPerPixel),
        "Luca Clicker",
        sf::Style::Default,
        sf::State::Windowed
    );

    window.setFramerateLimit(60);

    if (!font.openFromFile("assets/font/MightySouly-lxggD.ttf"))
        throw FontLoadingException("assets/font/MightySouly-lxggD.ttf");
}

Display::MenuResult Display::menu()
{
    const std::string savePath = "../data/savefile.txt";

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
            "../assets/textures/background.png"
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
            "../assets/textures/generic_banner_small_tintable.png"
        );

    std::vector<sf::Sprite> buttons;
    buttons.emplace_back(buttonTex); // New Game
    buttons.emplace_back(buttonTex); // Load Game

    for (auto& b : buttons)
        b.setScale({3.f, 3.f});

    float startY  = menuSize.y * 0.4f;
    float spacing = 100.f;

    for (std::size_t i = 0; i < buttons.size(); ++i) {
        const sf::FloatRect bounds = buttons[i].getGlobalBounds();
        buttons[i].setPosition({
            (menuSize.x - bounds.size.x) * 0.5f,
            startY + i * (bounds.size.y + spacing)
        });
    }

    const sf::Font& font =
        ResourceManager::instance().getFont(
            "../assets/font/MightySouly-lxggD.ttf"
        );

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
                                return MenuResult::NewGame;
                            }

                            if (i == 1) {
                                if (!saveExists()) {
                                    warning.setString("No saved game found!");
                                    warningClock.restart();
                                    break;
                                }
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
    const MenuResult result = menu();

    if (result == MenuResult::Exit)
        return;

    if (result == MenuResult::NewGame) {
        std::remove("../data/savefile.txt");
        gameManager.resetFromFile("../data/load.txt");
        initWindowAndFont();
    }

    if (result == MenuResult::LoadGame)
        gameManager.loadSavedGame();

    sf::Clock deltaClock;

    sf::Text header(font);
    header.setCharacterSize(HEADER_SIZE);
    header.setFillColor(sf::Color::White);

    sf::Text warning(font);
    warning.setCharacterSize(DETAIL_SIZE);
    warning.setFillColor(sf::Color::Red);

    while (window.isOpen())
    {
        const sf::Time dt = deltaClock.restart();
        gameManager.update(dt);

        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                using Scan = sf::Keyboard::Scan;

                switch (key->scancode)
                {
                    case Scan::Q: gameManager.saveGame(); window.close(); break;
                    case Scan::S: lastAction = 's'; break;
                    case Scan::U: lastAction = 'u'; break;
                    case Scan::D: lastAction = 'd'; break;
                    case Scan::B: lastAction = 'b'; break;
                    case Scan::Z: lastAction = 'z'; break;

                    case Scan::Num0: case Scan::Num1: case Scan::Num2:
                    case Scan::Num3: case Scan::Num4: case Scan::Num5:
                    case Scan::Num6: case Scan::Num7: case Scan::Num8:
                    case Scan::Num9:
                        selectedIndex = static_cast<int>(key->scancode) - static_cast<int>(Scan::Num1);
                        if (key->scancode == Scan::Num0) {
                            selectedIndex = 9;
                        } else if (selectedIndex < 0 || selectedIndex > 8) {
                            selectedIndex = -1;
                        }
                        break;

                    default: break;
                }
            }
        }

        if (lastAction != ' ')
        {
            if (selectedIndex >= 0 &&
                selectedIndex < static_cast<int>(displayToReal.size()))
            {
                const std::size_t idx = displayToReal[selectedIndex];
                Item& item = *gameManager.getItems()[idx];

                if (lastAction == 'z') {
                    warningMessage = gameManager.unlockItem(idx);
                    warningClock.restart();
                }
                else if (gameManager.isUnlocked(idx)) {
                    Delivery& delivery = gameManager.getDelivery()[idx];
                    switch (lastAction)
                    {
                        case 's': gameManager.runSellingLoop(item, idx); break;
                        case 'u': gameManager.upgrade(item); break;
                        case 'd': gameManager.startDelivery(item, delivery, static_cast<int>(idx)); break;
                        case 'b':
                        {
                            gameManager.useItem(idx);
                            break;
                        }
                        default: break;
                    }
                }
                else
                {
                    if (lastAction == 's' || lastAction == 'u' || lastAction == 'd' || lastAction == 'b')
                    {
                        warningMessage =
                            "Locked! Press Z to unlock (" +
                            std::to_string(static_cast<int>(item.getUnlockCost())) +
                            " RON)";
                        warningClock.restart();
                    }
                }
            }
            lastAction = ' ';
        }

        window.clear(sf::Color(25, 25, 25));

        if (const std::string msg = gameManager.popEventMessage(); !msg.empty()) {
            warningMessage = msg;
            warningClock.restart();
        }

        std::ostringstream top;
        top << "=========== LUCA CLICKER ===========\n"
            << "Money: " << gameManager.getPlayerMoney() << " RON\n"
            << "Selected item: " << (selectedIndex == -1 ? "None" : std::to_string(selectedIndex + 1)) << "\n";

        header.setString(top.str());
        header.setPosition({ LEFT_MARGIN, TOP_MARGIN });
        window.draw(header);

        float y = TOP_MARGIN + header.getGlobalBounds().size.y + 30.f;

        auto& allItems = gameManager.getItems();
        std::vector<bool> itemDrawn(allItems.size(), false);
        displayToReal.clear();
        int displayIndex = 0;

        for (size_t i = 0; i < allItems.size(); ++i) {
            if (itemDrawn[i]) continue;

            if (const auto* pastry = dynamic_cast<Pastry*>(allItems[i].get())) {
                displayToReal.push_back(i);
                float pastryHeight =
                    drawItemAndReturnHeight(*allItems[i], i, displayIndex++, LEFT_MARGIN, y);
                itemDrawn[i] = true;

                auto beverageIndex = static_cast<size_t>(-1);
                for (size_t j = 0; j < allItems.size(); ++j) {
                    if (auto* beverage = dynamic_cast<Beverage*>(allItems[j].get())) {
                        if (beverage->getTargetName() == pastry->getName()) {
                            beverageIndex = j;
                            break;
                        }
                    }
                }

                float beverageHeight = 0.f;
                if (beverageIndex != static_cast<size_t>(-1)) {
                    displayToReal.push_back(beverageIndex);
                    beverageHeight =
                        drawItemAndReturnHeight(*allItems[beverageIndex],
                                                beverageIndex,
                                                displayIndex++,
                                                window.getSize().x / 2.f,
                                                y);
                    itemDrawn[beverageIndex] = true;
                }

                y += std::max(pastryHeight, beverageHeight) + ITEM_SPACING;
            }
            else if (dynamic_cast<Sandwich*>(allItems[i].get())) {
                displayToReal.push_back(i);
                const float sandwichHeight =
                    drawItemAndReturnHeight(*allItems[i], i, displayIndex++, LEFT_MARGIN, y);
                itemDrawn[i] = true;
                y += sandwichHeight + ITEM_SPACING;
            }
        }

        for (size_t i = 0; i < allItems.size(); ++i) {
            if (!itemDrawn[i]) {
                displayToReal.push_back(i);
                const float itemHeight =
                    drawItemAndReturnHeight(*allItems[i], i, displayIndex++, LEFT_MARGIN, y);
                itemDrawn[i] = true;
                y += itemHeight + ITEM_SPACING;
            }
        }

        if (!warningMessage.empty()) {
            warning.setString(warningMessage);
            warning.setPosition({
                LEFT_MARGIN,
                static_cast<float>(window.getSize().y) - WARNING_Y_OFFSET - 50.f
            });
            window.draw(warning);

            if (warningClock.getElapsedTime().asSeconds() > 3.f)
                warningMessage.clear();
        }

        window.display();
    }
}


