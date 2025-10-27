#include <iostream>
#include <string>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <fstream>
#include <sstream>
#include <optional>

class FoodItem {
    std::string foodName;
    double baseIncome;
    double upgradeCost;
    double deliveryCost;
    double incomeMultiplier;
    double upgradeMultiplier;
    double unlockPrice; // NEW

public:
    // ================= Constructors =================
    FoodItem(std::string  foodName_,
             const double baseIncome_,
             const double upgradeCost_,
             const double deliveryCost_,
             const double incomeMultiplier_,
             const double upgradeMultiplier_,
             const double unlockPrice_)
        :   foodName(std::move(foodName_)),
            baseIncome(baseIncome_),
            upgradeCost(upgradeCost_),
            deliveryCost(deliveryCost_),
            incomeMultiplier(incomeMultiplier_),
            upgradeMultiplier(upgradeMultiplier_),
            unlockPrice(unlockPrice_){}

    FoodItem(const FoodItem& food) :
            foodName(food.foodName),
            baseIncome(food.baseIncome),
            upgradeCost(food.upgradeCost),
            deliveryCost(food.deliveryCost),
            incomeMultiplier(food.incomeMultiplier),
            upgradeMultiplier(food.upgradeMultiplier),
            unlockPrice(food.unlockPrice){}

    // ================= Operators =================

    FoodItem& operator=(const FoodItem& f) {
        foodName = f.foodName;
        baseIncome = f.baseIncome;
        upgradeCost = f.upgradeCost;
        deliveryCost = f.deliveryCost;
        incomeMultiplier = f.incomeMultiplier;
        upgradeMultiplier = f.upgradeMultiplier;
        unlockPrice = f.unlockPrice;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const FoodItem& f) {
        os<<f.foodName<<" a-> Pret:"<<f.baseIncome<<"  CostUpgrade:"<<f.upgradeCost<<"  DeliveryCost:"<<f.deliveryCost<<"  UnlockPrice:"<<f.unlockPrice<<"  MultiplicatorPret:"<<f.incomeMultiplier<<"  Multiplicator upgrade:"<<f.upgradeMultiplier<<std::endl;
        return os;
    }

    // ================= Getters =================
    [[nodiscard]] double getUnlockPrice() const { return unlockPrice; }
    [[nodiscard]] const std::string& getName() const { return foodName; }
    [[nodiscard]] double getBaseIncome() const { return baseIncome; }
    [[nodiscard]] double getUpgradeCost() const { return upgradeCost; }
    [[nodiscard]] double getDeliveryCost() const { return deliveryCost; }

    // ================= Functions =================
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
    // ================= Constructors =================
    Player(std::string  playerName_,const double money_) : playerName(std::move(playerName_)), money(money_) {}

    Player(const Player& p) : playerName(p.playerName) , money(p.money){}

    // ================= Operators =================
    Player& operator=(const Player& p) {
        playerName = p.playerName;
        money = p.money;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& op, const Player& p) {
        op<<p.playerName<<" "<<p.money;
        return op;
    }

    // ================= Getters/Setters =================
    [[nodiscard]] const double& getMoney() const {  return money;  }
    void setMoney(double const money_) { money = money_;}
};

class Delivery {
    std::string deliveryName;
    sf::Time timeInterval = sf::seconds(2.0f); // every 2 seconds by default
    bool running = false;
    friend class GameManager;

    // ================= Functions =================
private:
    void start(Player& player, FoodItem& fooditem) {
        player.setMoney(player.getMoney()-fooditem.getUnlockPrice());
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
    // ================= Constructors =================
    Delivery() = default;

    Delivery(std::string  name, const sf::Time& interval) : deliveryName(std::move(name)), timeInterval(interval) {}

    Delivery(const Delivery& del) : deliveryName(del.deliveryName), timeInterval(del.timeInterval){}

    // ================= Operators =================
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
    // ================= Constructors =================
    GameManager(const Player& player_, std::vector<FoodItem> foods_)
        : player(player_), foods(std::move(foods_)) {}

    GameManager(const GameManager& manager)
        : player(manager.player), foods(manager.foods) {}

    // ================= File Loading =================
    static GameManager loadFromFile(const std::string& filename, const Player& player) {
        std::ifstream file(filename);
        if (!file.is_open())
            throw std::runtime_error("Error: Unable to open file " + filename);

        std::vector<FoodItem> foods;
        std::string line;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string name;
            double baseIncome, upgradeCost, deliveryCost, incomeMultiplier, upgradeMultiplier, unlockPrice;

            // Skip empty lines
            if (line.empty()) continue;

            if (iss >> name >> baseIncome >> upgradeCost >> deliveryCost >>
                     unlockPrice >> upgradeMultiplier >> unlockPrice >> incomeMultiplier) {
                foods.emplace_back(name, baseIncome, upgradeCost, deliveryCost,
                                 unlockPrice, upgradeMultiplier, incomeMultiplier);
                     } else {
                         std::cerr << "Warning: Skipping malformed line: " << line << std::endl;
                     }
        }

        // Print loaded foods
        std::cout << "Loaded " << foods.size() << " food items:" << std::endl;
        for (const auto& food : foods) {
            std::cout << "  " << food.getName() << std::endl;
        }

        return { player, std::move(foods) };
    }

    // ================= Game Actions =================
    static void sell(const FoodItem& object, Player& player) {
        player.setMoney(player.getMoney() + object.getBaseIncome());
    }

    static void upgrade(FoodItem& object, Player& player) {
        if (player.getMoney() >= object.getUpgradeCost()) {
            player.setMoney(player.getMoney() - object.getUpgradeCost());
            object.update();
        }
    }

    static void delivery(FoodItem& foodItem, Player& player, Delivery& del) {
        if (player.getMoney() >= foodItem.getDeliveryCost()) {
            player.setMoney(player.getMoney() - foodItem.getDeliveryCost());
            del.start(player, foodItem);
        }
    }

    // ================= Getters =================
    [[nodiscard]] std::vector<FoodItem>& getFoods() { return foods; }


    // ================= Operators =================
    GameManager& operator=(const GameManager& manager) {
        if (this != &manager) {
            player = manager.player;
            foods = manager.foods;
        }
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const GameManager& manager) {
        os << "=== Game Manager ===\n";
        os << "Player: " << manager.player << " RON\n";
        os << "Food items:\n";
        for (const auto& food : manager.foods)
            os << "  " << food;
        return os;
    }
};


int main() {
    Player player("Stoicescu", 0.0);
    GameManager game_manager = GameManager::loadFromFile("resources/textfile.txt", player);

    std::vector<bool> unlocked;
    for (const auto& food : game_manager.getFoods())
        unlocked.push_back(player.getMoney() >= food.getUnlockPrice());

    unlocked[0] = true;

    Delivery glovo("Glovo", sf::seconds(2));

    sf::RenderWindow window;
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const unsigned int width = desktop.size.x;
    const unsigned int height = desktop.size.y;

    window.create(sf::VideoMode({width, height}, desktop.bitsPerPixel), "Luca Clicker", sf::Style::Default, sf::State::Windowed);
    window.setFramerateLimit(30);

    sf::Font font;
    if (!font.openFromFile("resources/font/MightySouly-lxggD.ttf")) { //
        std::cerr << "Failed to load font!\n";
        return 1;
    }

    sf::Text text(font, ""); //
    text.setCharacterSize(50);
    text.setFillColor(sf::Color::White);

    bool running = true;
    char lastAction = ' ';
    int selectedIndex = 1;

    while (window.isOpen() && running) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                running = false;
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                using Scan = sf::Keyboard::Scan;
                switch (keyPressed->scancode) {
                    case Scan::Q: running = false; break;
                    case Scan::S: lastAction = 's'; break;
                    case Scan::U: lastAction = 'u'; break;
                    case Scan::D: lastAction = 'd'; break;
                    case Scan::Z: lastAction = 'z'; break;
                    case Scan::Num1: case Scan::Num2: case Scan::Num3:
                    case Scan::Num4: case Scan::Num5: case Scan::Num6:
                    case Scan::Num7: case Scan::Num8: case Scan::Num9:
                        selectedIndex = static_cast<int>(keyPressed->scancode) - static_cast<int>(Scan::Num1) + 1; // conversie de coduri ca si in ascii
                        break;
                    default: break;
                }
            }
        }
        // Game logic
        if (lastAction != ' ') {
            if (selectedIndex >= 1 && static_cast<size_t>(selectedIndex) <= game_manager.getFoods().size()) {
                if (unlocked[selectedIndex - 1]) {
                    FoodItem& food = game_manager.getFoods()[selectedIndex - 1];
                    switch (lastAction) {
                        case 's': GameManager::sell(food, player); break;
                        case 'u': GameManager::upgrade(food, player); break;
                        case 'd': GameManager::delivery(food, player, glovo); break;
                        default: break;
                    }
                }
            }
            lastAction = ' ';
        }

        for (size_t i = 0; i < unlocked.size(); ++i) {
            if (!unlocked[i] && player.getMoney() >= game_manager.getFoods()[i].getUnlockPrice())
                unlocked[i] = true;
        }

        std::ostringstream buffer;
        buffer << "================ Luca Clicker ================\n";
        buffer << "Controls: [S] Sell | [U] Upgrade | [D] Delivery | [Q] Quit\n";
        buffer << "Use [1-9] to select food item.\n";
        buffer << "======================================================\n";
        buffer << "Money: " << player.getMoney() << " RON\n";
        buffer << "The currently selected item is:"<<selectedIndex<<std::endl;

        for (size_t i = 0; i < game_manager.getFoods().size(); ++i) {
            const auto& food = game_manager.getFoods()[i];
            if (unlocked[i])
                buffer << "[" << i + 1 << "] " << food.getName()
                       << " - Income: " << food.getBaseIncome()
                       << " | Upgrade: " << food.getUpgradeCost()
                       << " | Delivery: " << food.getDeliveryCost() << "\n";

            else
                buffer << "[" << i + 1 << "] (LOCKED - unlock at " << food.getUnlockPrice() << " RON)\n";
        }

        text.setString(buffer.str());
        sf::sleep(sf::milliseconds(50));
        window.clear(sf::Color(20, 20, 20));
        window.draw(text);
        window.display();
    }

    glovo.stop();
    std::cout << "Exiting game...\n";

}