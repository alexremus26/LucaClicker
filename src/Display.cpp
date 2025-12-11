#include "Display.h"
#include "Beverage.h"
#include <iostream>
#include <sstream>

Display::Display(GameManager &gm, Player &p)
        : gameManager(gm), player(p)
{
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(
            sf::VideoMode({desktop.size.x, desktop.size.y}, desktop.bitsPerPixel),
            "Luca Clicker", sf::Style::Default, sf::State::Windowed
    );

    window.setFramerateLimit(30);

    if (!font.openFromFile("resources/font/MightySouly-lxggD.ttf"))
        std::cerr << "Failed to load font!\n";
}

Display::Display(const Display &other)
        : gameManager(other.gameManager), player(other.player)
{}

Display::~Display() = default;

Display& Display::operator=(const Display &other) {
    if (this != &other) {
        gameManager = other.gameManager;
        player = other.player;
        selectedIndex = other.selectedIndex;
        lastAction = other.lastAction;
    }
    return *this;
}

std::ostream& operator<<(std::ostream &os, const Display &d) {
    os << d.gameManager << " " << d.player;
    return os;
}

void Display::handleUnlock(std::size_t index)
{
    switch (gameManager.unlockItem(index))
    {
        case 0:
            warningMessage = "Unlocked " + gameManager.getItems()[index]->getName() + "!";
            break;

        case 1:
            warningMessage = "Item already unlocked!";
            break;

        case 2:
            warningMessage = "Not enough money to unlock!";
            break;

        case 3:
            warningMessage = "Invalid index!";
            break;

        default:
            warningMessage = "Unknown error!";
            break;
    }

    warningClock.restart();
}

void Display::run()
{
    sf::Text mainText(font, "");
    mainText.setCharacterSize(50);
    mainText.setFillColor(sf::Color::White);

    sf::Text warnText(font, "");
    warnText.setCharacterSize(50);
    warnText.setFillColor(sf::Color::Red);

    while (window.isOpen())
    {
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

                    case Scan::Num1: case Scan::Num2: case Scan::Num3:
                    case Scan::Num4: case Scan::Num5: case Scan::Num6:
                    case Scan::Num7: case Scan::Num8: case Scan::Num9:
                        selectedIndex =
                            static_cast<int>(key->scancode) -
                            static_cast<int>(Scan::Num1) + 1;
                        break;

                    default: break;
                }
            }
        }

        if (lastAction != ' ')
        {
            if (selectedIndex > 0 &&
                selectedIndex <= static_cast<int>(gameManager.getItems().size()))
            {
                std::size_t idx = selectedIndex - 1;

                Item& item = *gameManager.getItems()[idx];
                Delivery& delivery = gameManager.getDelivery()[idx];

                if (lastAction == 'z')
                {
                    handleUnlock(idx);
                }
                else if (gameManager.isUnlocked(idx))
                {
                    switch (lastAction)
                    {
                        case 's':
                            gameManager.sell(item);
                            break;

                        case 'u':
                            gameManager.upgrade(item);
                            break;

                        case 'd':
                            gameManager.startDelivery(item, delivery, static_cast<int>(idx));
                            break;

                        case 'b':
                        {
                            auto* bev = dynamic_cast<Beverage*>(&item);
                            if (!bev) {
                                warningMessage = "This is not a beverage!";
                                warningClock.restart();
                                break;
                            }

                            double cost = bev->getUnlockCost() * 0.5;
                            if (player.getMoney() < cost) {
                                warningMessage = "Not enough money!";
                                warningClock.restart();
                                break;
                            }

                            player.setMoney(player.getMoney() - cost);

                            if (bev->getTarget() == "ALL")
                            {
                                for (auto& it : gameManager.getItems())
                                    bev->applyToOne(*it);
                            }
                            else
                            {
                                for (auto& it : gameManager.getItems())
                                    if (it->getName() == bev->getTarget())
                                        bev->applyToOne(*it);
                            }

                            warningMessage = "Beverage used!";
                            warningClock.restart();
                            break;
                        }

                        default:
                            break;
                    }
                }
                else
                {
                    warningMessage =
                        "Locked! Press Z to unlock (" +
                        std::to_string(static_cast<int>(item.getUnlockCost())) +
                        " RON)";
                    warningClock.restart();
                }
            }

            lastAction = ' ';
        }

        std::ostringstream out;

        out << "================ Luca Clicker =========================\n";
        out << "Controls:\n";
        out << " [S] Sell | [U] Upgrade | [D] Delivery | [B] Beverage\n";
        out << " [Z] Unlock | [Q] Quit\n";
        out << "=======================================================\n";
        out << "Money: " << player.getMoney() << "\n";
        out << "Selected item: " << selectedIndex << "\n\n";

        for (std::size_t i = 0; i < gameManager.getItems().size(); ++i)
        {
            Item& item = *gameManager.getItems()[i];
            Delivery& delivery = gameManager.getDelivery()[i];

            bool isBeverage = (dynamic_cast<Beverage*>(&item) != nullptr);

            if (gameManager.isUnlocked(i))
            {
                out << "[" << i + 1 << "] " << item.getName()
                    << (isBeverage ? " (Beverage)\n" : " (Pastry)\n");

                if (isBeverage)
                {
                    out << "   Effects: " << item.getEffectDescription() << "\n";
                }
                else
                {
                    out << "   Income: " << item.getBaseIncome()
                        << " | Upgrade: " << item.getUpgradeCost()
                        << " | Delivery: " << delivery.getUnlockCost() << "\n";
                }
            }
            else
            {
                out << "[" << i + 1 << "] LOCKED (Unlock cost: "
                    << item.getUnlockCost() << " RON)\n";
            }
        }

        mainText.setString(out.str());
        mainText.setPosition({20.f, 20.f});

        if (!warningMessage.empty())
        {
            warnText.setString(warningMessage);
            warnText.setPosition({50.f, window.getSize().y - 200.f});
        }

        if (!warningMessage.empty() &&
            warningClock.getElapsedTime().asSeconds() > 3)
        {
            warningMessage.clear();
        }

        window.clear(sf::Color(25, 25, 25));
        window.draw(mainText);

        if (!warningMessage.empty())
            window.draw(warnText);

        window.display();
    }
}
