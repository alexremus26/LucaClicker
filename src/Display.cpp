#include "Display.h"
#include "Beverage.h"

#include <iostream>
#include <sstream>


Display::Display(GameManager& gm, Player& p)
    : gameManager(gm), player(p)
{
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(
        sf::VideoMode({ desktop.size.x, desktop.size.y }, desktop.bitsPerPixel),
        "Luca Clicker",
        sf::Style::Default,
        sf::State::Windowed
    );

    window.setFramerateLimit(60);

    if (!font.openFromFile("resources/font/MightySouly-lxggD.ttf"))
        std::cerr << "Failed to load font!\n";
}

Display::Display(const Display& other)
    : gameManager(other.gameManager),
      player(other.player),
      selectedIndex(other.selectedIndex),
      lastAction(other.lastAction)
{}

Display::~Display() = default;

Display& Display::operator=(const Display& other)
{
    if (this != &other) {
        gameManager   = other.gameManager;
        player        = other.player;
        selectedIndex = other.selectedIndex;
        lastAction    = other.lastAction;
    }
    return *this;
}


std::ostream& operator<<(std::ostream& os, const Display& d)
{
    os << d.gameManager;
    return os;
}




void Display::handleUnlock(std::size_t index)
{
    switch (gameManager.unlockItem(index))
    {
        case 0: warningMessage = "Unlocked item!"; break;
        case 1: warningMessage = "Item already unlocked!"; break;
        case 2: warningMessage = "Not enough money!"; break;
        case 3: warningMessage = "Invalid item index!"; break;
        default: warningMessage = "Unknown error!"; break;
    }

    warningClock.restart();
}

void Display::drawProgressBar(const float progress, float x, float y)
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


void Display::run()
{
    sf::Text header(font);
    header.setCharacterSize(HEADER_SIZE);
    header.setFillColor(sf::Color::White);

    sf::Text warning(font);
    warning.setCharacterSize(DETAIL_SIZE);
    warning.setFillColor(sf::Color::Red);

    while (window.isOpen())
    {
        // events
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
        const std::size_t idx = selectedIndex - 1;
        Delivery& delivery = gameManager.getDelivery()[idx];

        // actions
        if (lastAction != ' ')
        {
            if (selectedIndex > 0 &&
                selectedIndex <= static_cast<int>(gameManager.getItems().size()))
            {
                Item& item = *gameManager.getItems()[idx];

                if (lastAction == 'z') {
                    handleUnlock(idx);
                }
                else if (gameManager.isUnlocked(idx)) {
                    switch (lastAction)
                    {
                        case 's': gameManager.runSellingLoop(item, idx); break;
                        case 'u': gameManager.upgrade(item); break;
                        case 'd': gameManager.startDelivery(item, delivery, static_cast<int>(idx)); break;

                        case 'b':
                        {
                            auto* const bev = dynamic_cast<Beverage*>(&item);
                            if (!bev) {
                                warningMessage = "Not a beverage!";
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

                            if (bev->getTarget() == "ALL") {
                                for (auto& it : gameManager.getItems())
                                    bev->applyToOne(*it);
                            } else {
                                for (auto& it : gameManager.getItems())
                                    if (it->getName() == bev->getTarget())
                                        bev->applyToOne(*it);
                            }

                            warningMessage = "Beverage used!";
                            warningClock.restart();
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

        // draw
        window.clear(sf::Color(25, 25, 25));

        std::ostringstream top;
        top << "=========== LUCA CLICKER ===========\n"
            << "Money: " << player.getMoney() << " RON\n"
            << "Selected item: " << selectedIndex << "\n";

        header.setString(top.str());
        header.setPosition({ LEFT_MARGIN, TOP_MARGIN });
        window.draw(header);

        float y = TOP_MARGIN + header.getGlobalBounds().size.y + 30.f;

        // items
        for (std::size_t i = 0; i < gameManager.getItems().size(); ++i)
        {
            Item& item = *gameManager.getItems()[i];
            bool isBeverage = dynamic_cast<Beverage*>(&item) != nullptr;

            sf::Text title(font);
            title.setCharacterSize(TITLE_SIZE);
            title.setFillColor(
                selectedIndex == static_cast<int>(i + 1)
                ? sf::Color(255, 220, 120)
                : sf::Color::White
            );

            if (gameManager.isUnlocked(i)) {
                title.setString(
                    "[" + std::to_string(i + 1) + "] " +
                    item.getName() +
                    (isBeverage ? " (Beverage)" : " (Pastry)")
                );
            } else {
                title.setString(
                    "[" + std::to_string(i + 1) +
                    "] LOCKED - Cost: " +
                    std::to_string(static_cast<int>(item.getUnlockCost())) + " RON"
                );
            }

            title.setPosition({ LEFT_MARGIN, y });
            window.draw(title);

            float blockHeight = title.getGlobalBounds().size.y;

            sf::Text details(font);
            details.setCharacterSize(DETAIL_SIZE);
            details.setFillColor(sf::Color(200, 200, 200));
            std::ostringstream oss;
            item.print(oss);

            if (gameManager.isUnlocked(i)) {
                if (isBeverage) {
                    details.setString("   Effects: " + item.getEffectDescription());
                } else {
                    details.setString(oss.str());
                }
            } else {
                details.setString("   Press Z to unlock");
            }

            details.setPosition({ LEFT_MARGIN, y + blockHeight + 6.f });
            window.draw(details);

            blockHeight += details.getGlobalBounds().size.y + 6.f;

            float progress = gameManager.getSellProgress(i);
            if (progress > 0.f) {
                drawProgressBar(
                    progress,
                    LEFT_MARGIN,
                    y + blockHeight + 6.f
                );
                blockHeight += PROGRESS_HEIGHT + 6.f;
            }

            y += blockHeight + ITEM_SPACING;
        }

        // warning
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
