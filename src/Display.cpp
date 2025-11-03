#include "Display.h"
#include <iostream>

Display::Display(GameManager &gm, Player &p) : gameManager(gm), player(p) {
    for (const auto& food : gameManager.getFoods())
        unlocked.push_back(player.getMoney() >= food.getUnlockCost());
    unlocked[0] = true; // First item starts unlocked

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

Display::~Display(){std::cout<<"Display a fost distrus! \n";}

Display &Display::operator=(const Display &other) {
    gameManager = other.gameManager;
    player = other.player;
    lastAction = other.lastAction;
    selectedIndex = other.selectedIndex;
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

    while (window.isOpen() && running) {
        // Handle events
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                running = false;
                window.close();
            }

            // Process keyboard input
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                using Scan = sf::Keyboard::Scan;
                switch (keyPressed->scancode) {
                    case Scan::Q: gameManager.saveGame(); running = false; break;
                    case Scan::S: lastAction = 's'; break;
                    case Scan::U: lastAction = 'u'; break;
                    case Scan::D: lastAction = 'd'; break;
                    case Scan::Num1: case Scan::Num2: case Scan::Num3:
                    case Scan::Num4: case Scan::Num5: case Scan::Num6:
                    case Scan::Num7: case Scan::Num8: case Scan::Num9:
                        selectedIndex = static_cast<int>(keyPressed->scancode)
                                      - static_cast<int>(Scan::Num1) + 1;
                        break;
                    default: break;
                }
            }
        }

        // Process player actions
        if (lastAction != ' ') {
            if (static_cast<size_t>(selectedIndex) <= gameManager.getFoods().size()) {
                FoodItem& food = gameManager.getFoods()[selectedIndex - 1];
                if (unlocked[selectedIndex - 1]) {
                    Delivery& delivery = gameManager.getDelivery()[selectedIndex - 1];
                    switch (lastAction) {
                        case 's': gameManager.sell(food); break;
                        case 'u': gameManager.upgrade(food); break;
                        case 'd': gameManager.startDelivery(food, delivery, selectedIndex); break;
                        default: ;
                    }
                    warningMessage.clear();
                } else {
                    // Show warning if item is locked
                    warningMessage = "Cannot sell or upgrade '" + food.getFoodName() +
                                     "' (unlock cost: " + std::to_string(static_cast<int>(food.getUnlockCost())) + " RON)";
                    warningClock.restart();
                }
            }
            lastAction = ' ';
        }

        // Check for newly unlocked items
        for (size_t i = 0; i < unlocked.size(); ++i)
            if (!unlocked[i] && player.getMoney() >= gameManager.getFoods()[i].getUnlockCost())
                unlocked[i] = true;

        std::ostringstream buffer;
        buffer << "================ Luca Clicker =========================\n";
        buffer << "Controls: [S] Sell | [U] Upgrade | [D] Delivery | [Q] Quit\n";
        buffer << "Use [1-9] to select a food item.\n";
        buffer << "======================================================\n";
        buffer << "Money: " << player.getMoney() << " RON\n";
        buffer << "Currently selected item: " << selectedIndex << "\n\n";

        // Display food items and their status
        for (size_t i = 0; i < gameManager.getFoods().size(); ++i) {
            const auto& food = gameManager.getFoods()[i];
            const auto& delivery = gameManager.getDelivery()[i];
            if (unlocked[i])
                buffer << "[" << i + 1 << "] " << food.getFoodName()
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





