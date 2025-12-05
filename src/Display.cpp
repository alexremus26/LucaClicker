#include "Display.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Display::Display(GameManager &gm, Player &p) : gameManager(gm), player(p) {

    for (const auto& item : gameManager.getItems())
        unlocked.push_back(player.getMoney() >= item->getUnlockCost());

    if (!unlocked.empty()) unlocked[0] = true; // Safety check

    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const unsigned int width = desktop.size.x;
    const unsigned int height = desktop.size.y;

    window.create(sf::VideoMode({width, height}, desktop.bitsPerPixel),
        "Luca Clicker", sf::Style::Default, sf::State::Windowed);

    window.setFramerateLimit(30);

    if (!font.openFromFile("resources/font/MightySouly-lxggD.ttf")) {
        std::cerr << "Failed to load font!\n";
    }
}

Display::Display(const Display &other) : gameManager(other.gameManager), player(other.player) {}

Display::~Display(){std::cout<<"Display destroyed!\n";}

Display &Display::operator=(const Display &other) {
    if (this != &other) { // Self-assignment check
        gameManager = other.gameManager;
        player = other.player;
        lastAction = other.lastAction;
        selectedIndex = other.selectedIndex;
    }
    return *this;
}

std::ostream &operator<<(std::ostream &os, const Display &d) {
    os << d.gameManager << " " << d.player;
    return os;
}

void Display::run() {

    // Setup main text display
    sf::Text text(font,"");
    text.setCharacterSize(50);
    text.setFillColor(sf::Color::White);

    // Setup warning text for error messages
    sf::Text warningText(font,"");
    warningText.setCharacterSize(50);
    warningText.setFillColor(sf::Color::Red);

    while (window.isOpen()) {
        // Handle events
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // Process keyboard input
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                using Scan = sf::Keyboard::Scan;
                switch (keyPressed->scancode) {
                    case Scan::Q: gameManager.saveGame(); window.close() ; break;
                    case Scan::S: lastAction = 's'; break;
                    case Scan::U: lastAction = 'u'; break;
                    case Scan::D: lastAction = 'd'; break;
                    case Scan::Num1: case Scan::Num2: case Scan::Num3:
                    case Scan::Num4: case Scan::Num5: case Scan::Num6:
                    case Scan::Num7: case Scan::Num8: case Scan::Num9:
                        selectedIndex = std::min(
                        (static_cast<int>(keyPressed->scancode) - static_cast<int>(Scan::Num1) + 1),
                         static_cast<int>(gameManager.getItems().size()) );
                        break;
                    default: break;
                }
            }
        }

        // Process player actions
        if (lastAction != ' ') {
            if (selectedIndex > 0 && static_cast<size_t>(selectedIndex) <= gameManager.getItems().size()) {


                Item& item = *gameManager.getItems()[selectedIndex - 1];

                if (unlocked[selectedIndex - 1]) {
                    Delivery& delivery = gameManager.getDelivery()[selectedIndex - 1];
                    switch (lastAction) {
                        case 's': gameManager.sell(item); break;
                        case 'u': gameManager.upgrade(item); break;
                        case 'd': gameManager.startDelivery(item, delivery, selectedIndex - 1); break;
                        default: ;
                    }
                    warningMessage.clear();
                } else {
                    warningMessage = "Cannot sell or upgrade '" + item.getName() +
                                     "' (unlock cost: " + std::to_string(static_cast<int>(item.getUnlockCost())) + " RON)";
                    warningClock.restart();
                }
            }
            lastAction = ' ';
        }

        // Check for newly unlocked items
        for (size_t i = 0; i < unlocked.size(); ++i)
            if (!unlocked[i] && player.getMoney() >= gameManager.getItems()[i]->getUnlockCost())
                unlocked[i] = true;

        std::ostringstream buffer;
        buffer << "================ Luca Clicker =========================\n";
        buffer << "Controls: [S] Sell | [U] Upgrade | [D] Delivery | [Q] Quit\n";
        buffer << "Use [1-"<< static_cast<int>(gameManager.getItems().size()) << "] to select a food item.\n";
        buffer << "======================================================\n";
        buffer << "Money: " << player.getMoney() << " RON\n";
        buffer << "Currently selected item: " << selectedIndex << "\n\n";

        // Display food items and their status
        for (size_t i = 0; i < gameManager.getItems().size(); ++i) {
            const Item& food = *gameManager.getItems()[i];
            const auto& delivery = gameManager.getDelivery()[i];

            if (unlocked[i])
                buffer << "[" << i + 1 << "] " << food.getName()
                       << " - Income: " << food.getBaseIncome()
                       << " | Upgrade: " << food.getUpgradeCost()
                       << " | Delivery: " << delivery.getUnlockCost() << "\n";
            else
                buffer << "[" << i + 1 << "] (LOCKED - unlock at "
                       << food.getUnlockCost() << " RON)\n";
        }

        text.setString(buffer.str());
        text.setPosition(sf::Vector2f(20.f, 20.f));

        // Handle warning messages
        if (!warningMessage.empty()) {
            warningText.setString(warningMessage);
            sf::FloatRect bounds = warningText.getLocalBounds();
            const float textHeight = bounds.position.y + bounds.size.y;
            const float windowHeight = static_cast<float>(window.getSize().y);
            warningText.setPosition({20.f, windowHeight - textHeight - 250.f});
            warningText.setFillColor(sf::Color::Red);
        }

        // Hide warning after 3 seconds
        if (!warningMessage.empty() && warningClock.getElapsedTime().asSeconds() > 3)
            warningMessage.clear();

        // Render frame
        window.clear(sf::Color(20, 20, 20));
        window.draw(text);
        if (!warningMessage.empty())
            window.draw(warningText);
        window.display();

        sf::sleep(sf::milliseconds(50)); // Control game loop
    }

    gameManager.stopAllDeliveries();
    std::cout << "Exiting game...\n";
}