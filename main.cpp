#include <iostream>
#include <string>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <atomic>
#include <cmath>

class FoodItem {
    std::string foodName;
    double baseIncome;
    double upgradeCost;
    double deliveryCost;
    double incomeMultiplier;
    double upgradeMultiplier;

public:
    FoodItem(const std::string& foodName_,
             const double baseIncome_,
             const double upgradeCost_,
             const double deliveryCost_,
             const double incomeMultiplier_,
             const double upgradeMultiplier_)
        :   foodName(foodName_),
            baseIncome(baseIncome_),
            upgradeCost(upgradeCost_),
            deliveryCost(deliveryCost_),
            incomeMultiplier(incomeMultiplier_),
            upgradeMultiplier(upgradeMultiplier_) {}

    FoodItem(const FoodItem& food) :
            foodName(food.foodName),
            baseIncome(food.baseIncome),
            upgradeCost(food.upgradeCost),
            deliveryCost(food.deliveryCost),
            incomeMultiplier(food.incomeMultiplier),
            upgradeMultiplier(food.upgradeMultiplier)   {}

    FoodItem& operator=(const FoodItem& f) {
        foodName = f.foodName;
        baseIncome = f.baseIncome;
        upgradeCost = f.upgradeCost;
        deliveryCost = f.deliveryCost;
        incomeMultiplier = f.incomeMultiplier;
        upgradeMultiplier = f.upgradeMultiplier;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const FoodItem& f) {
        os<<f.foodName<<"->Pret:"<<f.baseIncome<<"  CostUpgrade:"<<f.upgradeCost<<"  DeliveryCost:"<<f.deliveryCost<<"  MultiplicatorPret:"<<f.incomeMultiplier<<"  Multiplicator upgrade:"<<f.upgradeMultiplier<<std::endl;
        return os;
    }

    [[nodiscard]] const std::string& getName() const { return foodName; }
    [[nodiscard]] double getBaseIncome() const { return baseIncome; }
    [[nodiscard]] double getUpgradeCost() const { return upgradeCost; }
    [[nodiscard]] double getDeliveryCost() const { return deliveryCost; }
    [[nodiscard]] double newIncome() const { return baseIncome * incomeMultiplier + baseIncome; }
    [[nodiscard]] double newUpgradeCost() const { return upgradeMultiplier * upgradeCost + upgradeCost; }


    void update() {
        upgradeCost = newUpgradeCost();
        baseIncome = newIncome();
    }

};

class Player {
    std::string playerName;
    double money;

public:
    Player(const std::string& playerName_,const double money_) : playerName(playerName_), money(money_) {}

    Player(const Player& p) : playerName(p.playerName) , money(p.money){}

    Player& operator=(const Player& p) {
        playerName = p.playerName;
        money = p.money;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& op, const Player& p) {
        op<<p.playerName<<" "<<p.money;
        return op;
    }

    [[nodiscard]] const double& getMoney() const {  return money;  }
    void setMoney(double const money_) { money = money_;}
};

class Delivery {
    std::string deliveryName;
    sf::Time timeInterval = sf::seconds(2.0f); // every 2 seconds by default
    std::atomic<bool> running{false}; // atomic flag to safely stop the thread
    friend class GameManager;
private:
    void start(Player& player, FoodItem& fooditem) {

        running = true;
        std::thread([this, &player, &fooditem]() {
            sf::Clock clock;
            while (running) {
                sf::Time elapsed = clock.getElapsedTime();
                if (elapsed.asSeconds() >= timeInterval.asSeconds()) {
                    player.setMoney(player.getMoney() + fooditem.getBaseIncome());
                    clock.restart();
                }
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(100ms); // avoid 100% CPU usage
            }
        }).detach(); // run thread in background
    }
public:
    void stop() {
        running = false;
    }
    Delivery() = default;

    Delivery(const std::string& name, const sf::Time& interval) : deliveryName(name), timeInterval(interval) {}

    Delivery(const std::string& name, const FoodItem& item)
        : deliveryName(name)
    {
        // Example: base income affects delivery time
        // higher income -> slower delivery
        float base = static_cast<float>(item.getBaseIncome());

        // You can tweak this formula however you want
        float seconds = std::sqrt(base) + 2;

        timeInterval = sf::seconds(seconds);
    } // constructor with timeinterval formula

    Delivery(const Delivery& del) : deliveryName(del.deliveryName), timeInterval(del.timeInterval){}

    Delivery& operator=(const Delivery& del) {
        deliveryName = del.deliveryName;
        timeInterval = del.timeInterval;
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& os, const Delivery& del) {
        os<<"Delivery:"<<del.deliveryName<<"  SaleRate:"<<del.timeInterval.asSeconds()<<std::endl;
        return os;
    }
};

class GameManager {
    Player player;
    std::vector<FoodItem> foods;

public:
    GameManager(const Player& player_,const std::vector<FoodItem>& foods_) : player(player_), foods(foods_) {}

    GameManager(const GameManager& manager) : player(manager.player), foods(manager.foods) {}

    GameManager& operator=(const GameManager& manager) {
        player = manager.player;
        foods = manager.foods;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const GameManager& manager) {
        os << "=== Game Manager ===\n";
        os << "Player: " << manager.player << "\n";
        os << "Food items:\n";

        for (const auto& food : manager.foods) {
            os << "  " << food;  // relies on FoodItem's operator<<
        }
        return os;
    }

    static void sell(const FoodItem& object, Player& player) {
        player.setMoney(player.getMoney()+object.getBaseIncome());
    }
    static void upgrade(FoodItem& object, Player& player) {
        if (player.getMoney() >= object.getUpgradeCost()) {
            player.setMoney(player.getMoney() - object.getUpgradeCost());
            object.update();
        }
    }
    static void delivery(FoodItem& fooditem, Player& player, Delivery& del) {
        if (player.getMoney() >= fooditem.getDeliveryCost()) {
            player.setMoney(player.getMoney() - fooditem.getDeliveryCost());
            del.start(player, fooditem);
        }
    }
};



int main() {
    // de adaugat vector gamemanager
    Player player{"Stoici", 1000};
    FoodItem f1{"Covrig   ", 1, 5,100, 1.5, 2};
    FoodItem f2{"Merdenea ", 10, 100,500, 1.5, 2};
    FoodItem f3{"Pizza    ", 20, 200,1000, 1.5, 2};
    FoodItem f4{"CovriLuca", 50, 300,5000, 1.5, 2};

    Delivery glovo("Glovo", f2);
    GameManager::delivery(f2,player,glovo);

    while (true) {
           // Display all food items
        std::cout << "================================================================================\n";
        std::cout << "Player money: " << player.getMoney() << "\n";
        std::cout << f1 << f2 << f3 << f4;
        std::cout << "================================================================================\n";
        std::cout << "Press [a] [b] [c] [d] to sell items, or [q] to quit:\n> ";

        char choice;
        std::cin >> choice;

        if (choice == 'q') break;

        switch (choice) {
            case 'a':
                GameManager::sell(f1, player);
                break;
            case 'b':
                GameManager::upgrade(f2, player);
                break;
            case 'c':
                GameManager::upgrade(f3, player);
                break;
            case 'd':
                GameManager::upgrade(f4, player);
                break;
            default:
                std::cout << "Invalid option!\n";
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(500ms);
                break;
        }
    }
    glovo.stop();
    std::cout << "Exiting game...\n";
}

    // while (true) {
    //     sf::Time elapsed = clock.getElapsedTime();
    //     std::cout << elapsed.asSeconds() << std::endl;
    //
    // }
    // Player stoici{"stoici",100};
    // std::string display = std::to_string(stoici.getMoney());
    // sf::RenderWindow window;
    // const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    // const unsigned int width = desktop.size.x;
    // const unsigned int height = desktop.size.y;
    //
    // window.create(sf::VideoMode({width, height}, desktop.bitsPerPixel), "Luca Clicker", sf::Style::Default, sf::State::Windowed);
    //
    // std::cout << "Fereastra a fost creată\n";
    // window.setVerticalSyncEnabled(true);
    //
    // sf::Font font;
    // if (!font.openFromFile("Font/times.ttf")) {
    //     std::cout << "Failed to load font!" << std::endl;
    // }
    //
    //
    //
    // while(window.isOpen()) {
    //     bool shouldExit = false;
    //
    //     while(const std::optional event = window.pollEvent()) {
    //         if (event->is<sf::Event::Closed>()) {
    //             window.close();
    //             std::cout << "Fereastra a fost închisă\n";
    //         }
    //         else if (event->is<sf::Event::Resized>()) {
    //             std::cout << "New width: " << window.getSize().x << '\n'
    //                       << "New height: " << window.getSize().y << '\n';
    //         }
    //         else if (event->is<sf::Event::KeyPressed>()) {
    //             const auto* keyPressed = event->getIf<sf::Event::KeyPressed>();
    //             std::cout << "Received key " << (keyPressed->scancode == sf::Keyboard::Scancode::X ? "X" : "(other)") << "\n";
    //             if(keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
    //                 shouldExit = true;
    //             }
    //         }
    //     }
    //     if(shouldExit) {
    //         window.close();
    //         std::cout << "Fereastra a fost închisă (shouldExit == true)\n";
    //         break;
    //     }
    //     using namespace std::chrono_literals;
    //     std::this_thread::sleep_for(300ms);
    //
    //     window.clear();
    //     sf::Text text(font); // a font is required to make a text object
    //
    //     // set the string to display
    //     text.setString(display);
    //     // set the character size
    //     text.setCharacterSize(100); // in pixels, not points!
    //     // set the color
    //     text.setFillColor(sf::Color::Red);
    //     // set the text style
    //     text.setStyle(sf::Text::Regular);
    //     // inside the main loop, between window.clear() and window.display()
    //     window.draw(text);
    //
    //     window.display();
    // }
    // Player player("pula",100.0);
    // FoodItem croissant("Croissant", 5.0, 20.0, 1.0, 1.2);
    // GameManager manager;
    //
    // std::cout << "Player starts with: " << player.getMoney() << "\n";
    //
    // manager.sell(croissant, player);
    // std::cout << "After sell: " << player.getMoney() << "\n";
    //
    // manager.upgrade(croissant, player);
    // std::cout << "After upgrade: " << player.getMoney() << "\n";

