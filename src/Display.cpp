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
        "../assets/textures/ProgressBarFull.png"
        );

    sf::Sprite outline(outlineTex);
    sf::Sprite fill(fillTex);

    outline.setPosition({x, y});
    fill.setPosition({x, y});

    const sf::Vector2u size = fillTex.getSize();
    const auto clippedWidth =
        static_cast<unsigned int>(static_cast<float>(size.x) * clampedProgress);

    fill.setTextureRect({
        {0, -5},
        {static_cast<int>(clippedWidth), static_cast<int>(size.y)}
    });

    window.draw(outline);
    window.draw(fill);
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
        const std::ifstream file(savePath);
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
    const MenuResult choice = menu();
    if (choice == MenuResult::Exit)
        return;

    if (choice == MenuResult::LoadGame) {
        try {
            if (!gameManager.loadSavedGame()) {
                std::cerr << "Warning: save file could not be loaded.\n";
            }
        } catch (const FileOpenException& e) {
            std::cerr << "Error loading saved game: " << e.what() << ". Starting a new game.\n";
        } catch (const SaveStateException& e) {
            std::cerr << "Error loading saved game: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "An error occurred while loading saved game: " << e.what() << std::endl;
        }
    }

    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(
        sf::VideoMode({desktop.size.x, desktop.size.y}, desktop.bitsPerPixel),
        "Luca Clicker",
        sf::Style::Default
    );
    window.setView(window.getDefaultView());

    const float winW = static_cast<float>(window.getSize().x);
    const float winH = static_cast<float>(window.getSize().y);

    // Text: Sell/Upgrade
    auto makeSmallLabel = [&](const std::string& str,
                              const sf::FloatRect& bounds)
    {
        sf::Text t(font, str);
        t.setFillColor(sf::Color::Black);

        const auto size =
            static_cast<unsigned int>(bounds.size.y * 0.22f);
        t.setCharacterSize(size);

        t.setPosition({
            bounds.position.x + bounds.size.x * 0.1f,
            bounds.position.y + bounds.size.y * 0.12f
        });

        return t;
    };

    // Values
    auto makeValueLabel = [&](const std::string& str,
                              const sf::FloatRect& bounds)
    {
        sf::Text t(font, str);
        t.setFillColor(sf::Color::Black);

        const auto size =
            static_cast<unsigned int>(bounds.size.y * 0.25f);
        t.setCharacterSize(size);

        t.setOrigin(t.getLocalBounds().getCenter());
        t.setPosition(bounds.getCenter());
        t.move({0,20});

        return t;
    };

    sf::Sound soundtrack(
        ResourceManager::instance().getSound(
            "../assets/audio/adventure_capitalist_theme_song.wav"
        )
    );
    soundtrack.setLooping(true);
    soundtrack.setVolume(50.f);
    soundtrack.play();

    sf::Text moneyText(font, "", 90);
    moneyText.setOutlineThickness(3.f);
    moneyText.setOutlineColor(sf::Color::Black);

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

    const std::vector<std::string> itemIconFiles = {
        "../assets/textures/Pretzel.png",
        "../assets/textures/Pizza.png",
        "../assets/textures/ApplePie.png",
        "../assets/textures/Strudel.png",
        "../assets/textures/CovriLuca.png"
    };
    const std::vector<std::string> beverageIconFiles = {
        "../assets/textures/Water.png",
        "../assets/textures/Coke.png",
        "../assets/textures/Coffee.png",
        "../assets/textures/Ayran.png",
        "../assets/textures/Matcha.png"
    };

    std::vector<sf::Sprite> holders, buyButtons, upgradeButtons, itemIcons,
                            beverageHolders, useButtons, beverageIcons;

    std::vector<sf::Text>   buyTexts, upgradeTexts, useTexts,
                            itemNameTexts, beverageNameTexts,
                            buyValueTexts, upgradeValueTexts, useValueTexts;

    std::vector<bool> buyHovered(5, false);
    std::vector<bool> upgradeHovered(5, false);
    std::vector<bool> useHovered(5, false);


    float baseScale = winH * 0.00025f;
    float buyScale = baseScale * 1.25f;
    float timeScale = baseScale * 0.85f;

    float leftX = winW * 0.04f;
    float startY = winH * 0.18f;
    float spacing = winH * 0.155f;
    float progressX = leftX + holderTex.getSize().x * baseScale + winW * 0.015f;
    float buyX = progressX;
    float timeX = buyX + buyTex.getSize().x * buyScale + winW * 0.01f;

    float secondColumnX = timeX + timeTex.getSize().x * timeScale + winW * 0.08f;
    float useButtonX    = secondColumnX + holderTex.getSize().x * baseScale + winW * 0.015f;


    for (int i = 0; i < 5; ++i) {
        float y = startY + i * spacing;

        sf::Sprite h(holderTex);
        h.setScale({baseScale, baseScale});
        h.setPosition({leftX, y});
        holders.push_back(h);

        sf::Sprite icon(
            ResourceManager::instance().getTexture(itemIconFiles[i])
        );

        float holderHeight = holderTex.getSize().y * baseScale;
        float iconScale = holderHeight * 0.5f / icon.getTexture().getSize().y;
        icon.setScale({iconScale, iconScale});

        icon.setOrigin(icon.getLocalBounds().getCenter());
        icon.setPosition(holders.back().getGlobalBounds().getCenter());
        icon.move({0, -30});

        itemIcons.push_back(icon);

        sf::Text nameText(font, gameManager.getItems()[i]->getName(), 26);
        nameText.setFillColor(sf::Color(60, 60, 60));
        nameText.setOrigin(nameText.getLocalBounds().getCenter());

        sf::FloatRect hBounds = holders.back().getGlobalBounds();
        nameText.setPosition({
            hBounds.getCenter().x,
            hBounds.position.y + hBounds.size.y * 0.82f
        });

        itemNameTexts.push_back(nameText);

        sf::Sprite b(buyTex);
        b.setScale({buyScale, buyScale});
        b.setPosition({buyX, y + winH * 0.055f});
        buyButtons.push_back(b);

        sf::FloatRect bBounds = b.getGlobalBounds();

        buyTexts.push_back(
            makeSmallLabel("SELL", bBounds)
        );

        buyValueTexts.push_back(
            makeValueLabel("0", bBounds)
        );

        sf::Sprite t(timeTex);
        t.setScale({timeScale, timeScale});
        t.setPosition({timeX, y + winH * 0.062f});
        upgradeButtons.push_back(t);

        sf::FloatRect tBounds = t.getGlobalBounds();

        upgradeTexts.push_back(
            makeSmallLabel("UPGRADE", tBounds)
        );

        sf::Text upgVal = makeValueLabel("0", tBounds);
        upgVal.setFillColor(sf::Color::White);
        upgradeValueTexts.push_back(upgVal);

        sf::Sprite bh(holderTex);
        bh.setScale({baseScale, baseScale});
        bh.setPosition({secondColumnX, y});
        beverageHolders.push_back(bh);

        sf::Sprite bIcon(
            ResourceManager::instance().getTexture(beverageIconFiles[i])
        );

        float bHolderHeight = holderTex.getSize().y * baseScale;
        float bIconScale = bHolderHeight * 0.45f / bIcon.getTexture().getSize().y;
        bIcon.setScale({bIconScale, bIconScale});
        bIcon.setOrigin(bIcon.getLocalBounds().getCenter());
        bIcon.setPosition(beverageHolders.back().getGlobalBounds().getCenter());
        bIcon.move({0, -30});

        beverageIcons.push_back(bIcon);
        std::size_t beverageIndex = i + 5;
        sf::Text bName(font, gameManager.getItems()[beverageIndex]->getName(), 26);
        bName.setFillColor(sf::Color(60, 60, 60));
        bName.setOrigin(bName.getLocalBounds().getCenter());

        sf::FloatRect bhBounds = beverageHolders.back().getGlobalBounds();
        bName.setPosition({
            bhBounds.getCenter().x,
            bhBounds.position.y + bhBounds.size.y * 0.82f
        });

        beverageNameTexts.push_back(bName);

        sf::Sprite ub(buyTex);
        ub.setScale({buyScale, buyScale});
        ub.setPosition({useButtonX, y + winH * 0.055f});
        useButtons.push_back(ub);

        sf::FloatRect ubBounds = ub.getGlobalBounds();

        useTexts.push_back(
            makeSmallLabel("USE", ubBounds)
        );

        useValueTexts.push_back(
            makeValueLabel("0", ubBounds)
        );
    }

    while (window.isOpen()) {
        gameManager.updateSelling();

        while (auto e = window.pollEvent()) {
            if (e->is<sf::Event::Closed>()) {
                gameManager.saveGame();
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
                            }
                            else if (!gameManager.isSelling(i)) {
                                const auto& item = *gameManager.getItems()[i];
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
                    for (int i = 0; i < 5; ++i) {
                        if (useButtons[i].getGlobalBounds().contains(mouse)) {
                            std::size_t index = i + 5;

                            if (!gameManager.isUnlocked(index)) {
                                gameManager.unlockItem(index);
                            } else {
                                gameManager.useItem(index);
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
            useHovered[i] = useButtons[i].getGlobalBounds().contains(mouse);

        }

        double money = gameManager.getPlayerMoney();
        moneyText.setString("$ " + std::to_string(static_cast<long long>(money)));
        window.draw(moneyText);


        for (int i = 0; i < 5; ++i) {
            const Item& item = *gameManager.getItems()[i];

            bool unlocked = gameManager.isUnlocked(i);

            buyTexts[i].setString(unlocked ? "SELL" : "UNLOCK");

            double buyValue =
                unlocked ? item.sellPayout()
                         : item.getUnlockCost();

            buyValueTexts[i].setString(
                std::to_string(static_cast<long long>(buyValue))
            );

            if (gameManager.isUnlocked(i)) {
                upgradeTexts[i].setString("UPGRADE");

                double upgCost = item.getUpgradeCost();
                upgradeValueTexts[i].setString(
                    upgCost > 0 ? std::to_string(static_cast<long long>(upgCost)) : "-"
                );
            } else {
                upgradeTexts[i].setString("");
                upgradeValueTexts[i].setString("");
            }

            std::size_t bevIndex = i + 5;
            const Item& bev = *gameManager.getItems()[bevIndex];

            if (!gameManager.isUnlocked(bevIndex)) {
                useTexts[i].setString("UNLOCK");
                useValueTexts[i].setString(
                    std::to_string(static_cast<long long>(bev.getUnlockCost()))
                );
            }
            else if (!bev.isUsable()) {
                useTexts[i].setString("");
                useValueTexts[i].setString("");
            }
            else {
                useTexts[i].setString("USE");
                useValueTexts[i].setString(
                    std::to_string(static_cast<long long>(bev.getUseCost()))
                );
            }

            window.draw(holders[i]);
            window.draw(itemNameTexts[i]);
            window.draw(itemIcons[i]);

            buyButtons[i].setColor(buyHovered[i] ? sf::Color(255, 230, 160) : sf::Color::White);
            upgradeButtons[i].setColor(upgradeHovered[i] ? sf::Color(200, 200, 255) : sf::Color::White);

            window.draw(buyButtons[i]);

            window.draw(buyTexts[i]);
            window.draw(buyValueTexts[i]);

            window.draw(upgradeButtons[i]);

            window.draw(upgradeTexts[i]);
            window.draw(upgradeValueTexts[i]);
            drawProgressBar(gameManager.anyProgress(i), progressX, startY + i * spacing);
            if (gameManager.isSelling(i)) {
                buyButtons[i].setColor(sf::Color(180, 180, 180));
            }
        }
            for (int i = 0; i < 5; ++i) {
                window.draw(beverageHolders[i]);
                window.draw(beverageNameTexts[i]);
                window.draw(beverageIcons[i]);

                useButtons[i].setColor(
                    useHovered[i] ? sf::Color(160, 230, 255) : sf::Color::White
                );

                window.draw(useButtons[i]);

                window.draw(useTexts[i]);
                window.draw(useValueTexts[i]);
            }
        window.display();
    }
}