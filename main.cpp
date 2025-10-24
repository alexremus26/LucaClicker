#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <bits/std_thread.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/VideoMode.hpp>


class FoodItem {
    std::string foodName;
    double baseIncome;
    double upgradeCost;
    double incomeMultiplier;
    double upgradeMultiplier;

public:
    FoodItem(const std::string& foodName_,
             const double baseIncome_,
             const double upgradeCost_,
             const double incomeMultiplier_,
             const double upgradeMultiplier_)
        : foodName(foodName_), baseIncome(baseIncome_),
          upgradeCost(upgradeCost_), incomeMultiplier(incomeMultiplier_),
          upgradeMultiplier(upgradeMultiplier_) {}

    double getBaseIncome() const { return baseIncome; }
    double getUpgradeCost() const { return upgradeCost; }
    double newIncome() const { return baseIncome * incomeMultiplier + baseIncome; }
    double newUpgradeCost() const { return upgradeMultiplier * upgradeCost + upgradeCost; }

    void upgrade() {
        incomeMultiplier *= 1.5;
        upgradeCost *= 2.0;
    }

    const std::string& getName() const { return foodName; }
};

class Player {
    std::string playerName;
    std::vector<FoodItem> ownedItems;
    double money;

public:
    explicit Player(double const money_) : money(money_) {}
    double getMoney() const { return money; }
    void setMoney(double const money_) { money = money_; }
};

class GameManager {
public:
    static void buy(const FoodItem& object, Player& player) {
        double newMoney = player.getMoney() + object.getBaseIncome();
        player.setMoney(newMoney);
    }

    static void upgrade(FoodItem& object, Player& player) {
        if (player.getMoney() >= object.getUpgradeCost()) {
            player.setMoney(player.getMoney() - object.getUpgradeCost());
            object.upgrade();
        }
    }
};


int main() {

    sf::RenderWindow window;
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const unsigned int width = desktop.size.x;
    const unsigned int height = desktop.size.y;

    window.create(sf::VideoMode({width, height}, desktop.bitsPerPixel), "Luca Clicker", sf::Style::Default, sf::State::Windowed);

    std::cout << "Fereastra a fost creată\n";
    window.setVerticalSyncEnabled(true);

    while(window.isOpen()) {
        bool shouldExit = false;

        while(const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                std::cout << "Fereastra a fost închisă\n";
            }
            else if (event->is<sf::Event::Resized>()) {
                std::cout << "New width: " << window.getSize().x << '\n'
                          << "New height: " << window.getSize().y << '\n';
            }
            else if (event->is<sf::Event::KeyPressed>()) {
                const auto* keyPressed = event->getIf<sf::Event::KeyPressed>();
                std::cout << "Received key " << (keyPressed->scancode == sf::Keyboard::Scancode::X ? "X" : "(other)") << "\n";
                if(keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    shouldExit = true;
                }
            }
        }
        if(shouldExit) {
            window.close();
            std::cout << "Fereastra a fost închisă (shouldExit == true)\n";
            break;
        }
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(300ms);

        window.clear();
        window.display();
    }



    // Player player(100.0);
    // FoodItem croissant("Croissant", 5.0, 20.0, 1.0, 1.2);
    // GameManager manager;
    //
    // std::cout << "Player starts with: " << player.getMoney() << "\n";
    //
    // manager.buy(croissant, player);
    // std::cout << "After buy: " << player.getMoney() << "\n";
    //
    // manager.upgrade(croissant, player);
    // std::cout << "After upgrade: " << player.getMoney() << "\n";
}
