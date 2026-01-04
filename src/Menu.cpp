#include "Menu.h"
#include "ResourceManager.h"
#include "GameExceptions.h"

#include <fstream>
#include <iostream>

Menu::Menu(sf::RenderWindow &window)
    : window(window) {
    init();
}

void Menu::loadFont() {
    if (!font.openFromFile("assets/font/MightySouly-lxggD.ttf"))
        throw FontLoadingException("assets/font/MightySouly-lxggD.ttf");
}

bool Menu::saveExists() {
    const std::ifstream file(SavePath);
    return file.good();
}

void Menu::deleteSave() {
    std::remove(SavePath);
}

void Menu::buildBackground() {
    background.emplace(ResourceManager::instance().getTexture("assets/textures/MenuBackground.png"));
    const sf::Vector2u bgSize = background->getTexture().getSize();
    background->setScale({
        static_cast<float>(MenuSize.x) / static_cast<float>(bgSize.x),
        static_cast<float>(MenuSize.y) / static_cast<float>(bgSize.y)
    });
}

void Menu::buildButtons() {
    const sf::Texture &buttonTex = ResourceManager::instance().getTexture("assets/textures/MenuButton.png");

    buttons.clear();
    buttons.emplace_back(buttonTex);
    buttons.emplace_back(buttonTex);

    for (auto &b: buttons)
        b.setScale({0.75f, 0.75f});

    constexpr float startY = static_cast<float>(MenuSize.y) * 0.4f;

    for (std::size_t i = 0; i < buttons.size(); ++i) {
        constexpr float spacing = 100.f;
        const sf::FloatRect bounds = buttons[i].getGlobalBounds();
        buttons[i].setPosition({
            (static_cast<float>(MenuSize.x) - bounds.size.x) * 0.5f,
            startY + static_cast<float>(i) * (bounds.size.y + spacing)
        });
    }
}

void Menu::buildLabels() {
    labels.clear();
    labels.emplace_back(font, "New Game", 50);
    labels.emplace_back(font, "Saved Game", 50);

    for (std::size_t i = 0; i < labels.size(); ++i) {
        labels[i].setFillColor(sf::Color(200, 200, 200));

        const sf::FloatRect tb = labels[i].getLocalBounds();
        labels[i].setOrigin(tb.getCenter());

        const sf::FloatRect bb = buttons[i].getGlobalBounds();
        labels[i].setPosition(bb.getCenter());
    }
}

void Menu::buildTitle() {
    title.emplace(font, "Luca Clicker", 160);
    title->setFillColor(sf::Color(200, 200, 200));

    const sf::FloatRect tb = title->getLocalBounds();
    title->setOrigin(tb.getCenter());

    title->setPosition({
        static_cast<float>(MenuSize.x) * 0.5f,
        static_cast<float>(MenuSize.y) * 0.2f
    });
}

void Menu::buildWarning() {
    warning.emplace(font, "", 32);
    warning->setFillColor(sf::Color::Red);

    const sf::FloatRect wb = warning->getLocalBounds();
    warning->setOrigin(wb.getCenter());

    warning->setPosition({
        static_cast<float>(MenuSize.x) * 0.5f,
        static_cast<float>(MenuSize.y) * 0.3f
    });
}

void Menu::init() {
    loadFont();
    buildBackground();
    buildButtons();
    buildLabels();
    buildTitle();
    buildWarning();
}

void Menu::pollEvents() {
    const sf::Vector2f mouse =
            window.mapPixelToCoords(sf::Mouse::getPosition(window));

    while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            result = Result::Exit;
            return;
        }

        if (const auto *me = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (me->button != sf::Mouse::Button::Left)
                continue;

            for (std::size_t i = 0; i < buttons.size(); ++i) {
                if (!buttons[i].getGlobalBounds().contains(mouse))
                    continue;

                if (i == 0) {
                    if (saveExists())
                        deleteSave();
                    result = Result::NewGame;
                    window.close();
                    return;
                }

                if (i == 1) {
                    if (!saveExists()) {
                        warning->setString("No saved game found!");
                        warning->setOrigin(warning->getLocalBounds().getCenter());
                        warningClock.restart();
                        break;
                    }
                    result = Result::LoadGame;
                    window.close();
                    return;
                }
            }
        }
    }
}

void Menu::update() {
    const sf::Vector2f mouse =
            window.mapPixelToCoords(sf::Mouse::getPosition(window));

    for (auto &b: buttons) {
        b.setColor(b.getGlobalBounds().contains(mouse) ? sf::Color(255, 215, 0) : sf::Color::White);
    }
}

void Menu::draw() const {
    window.clear();

    if (background) window.draw(*background);
    if (title) window.draw(*title);

    if (warning && !warning->getString().isEmpty() && warningClock.getElapsedTime().asSeconds() < 2.5f)
        window.draw(*warning);

    for (const auto &b: buttons)
        window.draw(b);

    for (const auto &t: labels)
        window.draw(t);

    window.display();
}

void Menu::show() {
    while (window.isOpen()) {
        pollEvents();
        if (!window.isOpen())
            break;
        update();
        draw();
    }
}

bool Menu::wantsLoadGame() const {
    return result == Result::LoadGame;
}

bool Menu::wantsExit() const {
    return result == Result::Exit;
}
