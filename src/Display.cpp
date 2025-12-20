#include "Display.h"
#include "Beverage.h"
#include "Pastry.h"
#include "Sandwich.h"

#include <iostream>
#include <sstream>

Display::Display(GameManager& manager)
    : gameManager(manager)
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
            "[" + std::to_string(displayIndex) + "] " +
            item.getName() + " (" + item.getType() + ")"
        );
    } else {
        title.setString(
            "[" + std::to_string(displayIndex) +
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

void Display::run()
{
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
                selectedIndex <= static_cast<int>(displayToReal.size()))
            {
                const std::size_t idx = displayToReal[selectedIndex - 1];
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
                            warningMessage = gameManager.popEventMessage();
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

        window.clear(sf::Color(25, 25, 25));

        if (const std::string msg = gameManager.popEventMessage(); !msg.empty()) {
            warningMessage = msg;
            warningClock.restart();
        }

        std::ostringstream top;
        top << "=========== LUCA CLICKER ===========\n"
            << "Money: " << gameManager.getPlayerMoney() << " RON\n"
            << "Selected item: " << selectedIndex << "\n";

        header.setString(top.str());
        header.setPosition({ LEFT_MARGIN, TOP_MARGIN });
        window.draw(header);

        float y = TOP_MARGIN + header.getGlobalBounds().size.y + 30.f;

        auto& allItems = gameManager.getItems();
        std::vector<bool> itemDrawn(allItems.size(), false);
        displayToReal.clear();
        int displayIndex = 1;

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
