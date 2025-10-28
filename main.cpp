#include <iostream>
#include <string>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <fstream>
#include <optional>


class FoodItem {
    std::string foodName;
    double baseIncome;
    double upgradeCost;
    double unlockCost;
    double incomeMultiplier;
    double upgradeMultiplier;

public:
    // ================= Constructors =================
    FoodItem(std::string  foodName_,
             const double baseIncome_,
             const double upgradeCost_,
             const double incomeMultiplier_,
             const double upgradeMultiplier_,
             const double unlockCost_)
        :   foodName(std::move(foodName_)),
            baseIncome(baseIncome_),
            upgradeCost(upgradeCost_),
            unlockCost(unlockCost_),
            incomeMultiplier(incomeMultiplier_),
            upgradeMultiplier(upgradeMultiplier_){}

    FoodItem(const FoodItem& foodItem) :
            foodName(foodItem.foodName),
            baseIncome(foodItem.baseIncome),
            upgradeCost(foodItem.upgradeCost),
            unlockCost(foodItem.unlockCost),
            incomeMultiplier(foodItem.incomeMultiplier),
            upgradeMultiplier(foodItem.upgradeMultiplier){}

    // ================= Operators =================

    FoodItem& operator=(const FoodItem& foodItem) {
        foodName = foodItem.foodName;
        baseIncome = foodItem.baseIncome;
        upgradeCost = foodItem.upgradeCost;
        incomeMultiplier = foodItem.incomeMultiplier;
        upgradeMultiplier = foodItem.upgradeMultiplier;
        unlockCost = foodItem.unlockCost;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const FoodItem& foodItem) {
        ostream<<foodItem.foodName<<" a-> Pret:"<<foodItem.baseIncome<<"  CostUpgrade:"<<foodItem.upgradeCost<<"  UnlockCost:"<<foodItem.unlockCost<<"  MultiplicatorPret:"<<foodItem.incomeMultiplier<<"  Multiplicator upgrade:"<<foodItem.upgradeMultiplier<<std::endl;
        return ostream;
    }

    // ================= Getters =================
    [[nodiscard]] double getUnlockCost() const { return unlockCost; }
    [[nodiscard]] const std::string& getfoodName() const { return foodName; }
    [[nodiscard]] double getBaseIncome() const { return baseIncome; }
    [[nodiscard]] double getUpgradeCost() const { return upgradeCost; }

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

    Player(const Player& player) : playerName(player.playerName) , money(player.money){}

    // ================= Operators =================
    Player& operator=(const Player& player) {
        playerName = player.playerName;
        money = player.money;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Player& player) {
        os<<player.playerName<<" "<<player.money;
        return os;
    }

    // ================= Getters/Setters =================
    [[nodiscard]] const double& getMoney() const {  return money;  }
    void setMoney(double const money_) { money = money_;}
};

class Delivery {
    std::string deliveryName;
    double unlockDeliveryCost;
    sf::Time timeInterval = sf::seconds(2.0f);
    bool running = false;
    friend class GameManager;

    // ================= Functions =================
public:
    [[nodiscard]] bool canUnlock(const Player& player) const {return player.getMoney() >= unlockDeliveryCost;}
    // ================= Getters =================
    [[nodiscard]] const std::string& getDeliveryName() const { return deliveryName; }
    [[nodiscard]] double getUnlockCost() const { return unlockDeliveryCost; }
    [[nodiscard]] sf::Time getTimeInterval() const { return timeInterval; }
    [[nodiscard]] const double& getDeliveryUnlockCost() const{return unlockDeliveryCost;}
    // ================= Constructors =================
    Delivery(std::string deliveryName_, const double& unlockDeliveryCost_, const sf::Time& timeInterval_) : deliveryName(std::move(deliveryName_)), unlockDeliveryCost(unlockDeliveryCost_), timeInterval(timeInterval_){};

    Delivery(std::string  name, const double& unlockDeliveryCost_) : deliveryName(std::move(name)), unlockDeliveryCost(unlockDeliveryCost_) {}

    Delivery(const Delivery& delivery)
        : deliveryName(delivery.deliveryName),
          unlockDeliveryCost(delivery.unlockDeliveryCost),
          timeInterval(delivery.timeInterval),
          running(delivery.running) {}

    // ================= Operators =================
    Delivery& operator=(const Delivery& delivery) {
        deliveryName = delivery.deliveryName;
        timeInterval = delivery.timeInterval;
        unlockDeliveryCost = delivery.unlockDeliveryCost;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Delivery& delivery) {
        ostream<<"Delivery:"<<delivery.deliveryName<<"Unlock Cost:"<<delivery.unlockDeliveryCost<<"  SaleRate:"<<delivery.timeInterval.asSeconds()<<std::endl;
        return ostream;
    }
};

class GameManager {
    Player& player;
    std::vector<FoodItem> foodItems;
    std::vector<Delivery> deliveries;
    bool deliveryRunning = false;

private:
    void runDeliveryLoop(FoodItem& food, Delivery& delivery) const {
        std::thread([this, &food, &delivery]() {
            sf::Clock clock;
            while (deliveryRunning) {
                if (clock.getElapsedTime().asSeconds() >= delivery.getTimeInterval().asSeconds()) {
                    player.setMoney(player.getMoney() + food.getBaseIncome());
                    clock.restart();
                }
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(50ms);
            }
        }).detach();
    }

public:
    // ================= Constructors =================
    GameManager(Player& player_, std::vector<FoodItem> foodItem_, std::vector<Delivery> deliveries_)
         : player(player_), foodItems(std::move(foodItem_)), deliveries(std::move(deliveries_)) {}

    GameManager(const GameManager& gameManager)
        : player(gameManager.player), foodItems(gameManager.foodItems), deliveries(gameManager.deliveries) {}

    // ================= File Loading =================
    static GameManager loadFromFile(const std::string& fileName, Player& player) {
    std::ifstream file(fileName);
    if (!file.is_open())
        throw std::runtime_error("Error: Unable to open file " + fileName);

    std::vector<FoodItem> foodItems;
    std::vector<Delivery> deliveries;

    std::string line;

    std::string foodName, courierName;
    double baseIncome = 0, upgradeCost = 0, upgradeMultiplier = 0;
    double incomeMultiplier = 0, unlockFoodCost = 0, unlockDeliveryCost = 0;

    auto reset_fields = [&]() {
        foodName.clear();
        courierName.clear();
        baseIncome = upgradeCost = upgradeMultiplier = incomeMultiplier = unlockFoodCost = unlockDeliveryCost = 0;
    };

    while (std::getline(file, line)) {
        // Empty line -> commit current food + delivery
        if (line.empty()) {
            if (!foodName.empty()) {
                foodItems.emplace_back(
                    foodName,
                    baseIncome,
                    upgradeCost,
                    incomeMultiplier,
                    upgradeMultiplier,
                    unlockFoodCost
                );

                deliveries.emplace_back(courierName,unlockDeliveryCost);


                reset_fields();
            }
            continue;
        }

        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, ':')) {
            std::string value;
            std::getline(iss, value);

            if (!value.empty() && value[0] == ' ')
                value.erase(0, 1);

            if (key == "foodName") foodName = value;
            else if (key == "baseIncome") baseIncome = std::stod(value);
            else if (key == "upgradeCost") upgradeCost = std::stod(value);
            else if (key == "upgradeMultiplier") upgradeMultiplier = std::stod(value);
            else if (key == "incomeMultiplier") incomeMultiplier = std::stod(value);
            else if (key == "unlockFoodCost") unlockFoodCost = std::stod(value);
            else if (key == "courierName") courierName = value;
            else if (key == "unlockDeliveryCost") unlockDeliveryCost = std::stod(value);
        }
    }

    // If file doesn’t end with an empty line
    if (!foodName.empty()) {
        foodItems.emplace_back(
            foodName,
            baseIncome,
            upgradeCost,
            incomeMultiplier,
            upgradeMultiplier,
            unlockFoodCost
        );

        deliveries.emplace_back(courierName, unlockDeliveryCost);
    }
        std::cout << "Loaded " << foodItems.size() << " food items and couriers:\n";
        for (size_t i = 0; i < foodItems.size(); ++i) {
            const auto& food = foodItems[i];
            const auto& delivery = deliveries[i];

            std::cout << "=======================================================\n";
            std::cout << "Food Name: " << food.getfoodName() << "\n"
                      << "  Base Income: " << food.getBaseIncome() << "\n"
                      << "  Upgrade Cost: " << food.getUpgradeCost() << "\n"
                      << "  Unlock Food Cost: " << food.getUnlockCost() << "\n"
                      << "  Courier Name: " << delivery.deliveryName << "\n"
                      << "  Delivery Unlock Cost: " << delivery.getDeliveryUnlockCost() << "\n"
                      << "  Delivery Interval: " << delivery.timeInterval.asSeconds() << "s\n";
        }
             std::cout<< "=======================================================\n";


    return { player, std::move(foodItems), std::move(deliveries) };
}

    // ================= Game Actions =================
    static void sell(const FoodItem& foodItem, Player& player) {
        player.setMoney(player.getMoney() + foodItem.getBaseIncome());
    }

    static void upgrade(FoodItem& foodItem, Player& player) {
        if (player.getMoney() >= foodItem.getUpgradeCost()) {
            player.setMoney(player.getMoney() - foodItem.getUpgradeCost());
            foodItem.update();
        }
    }

    void startDelivery(FoodItem& foodItem, Delivery& delivery) {
        if (!deliveryRunning && delivery.canUnlock(player)) {
            player.setMoney(player.getMoney() - delivery.getDeliveryUnlockCost());
            deliveryRunning = true;
            runDeliveryLoop(foodItem, delivery);
        }
    }
    void stopDelivery() { deliveryRunning = false; }

    // ================= Getters =================
    [[nodiscard]] std::vector<FoodItem>& getFoods() { return foodItems; }
    [[nodiscard]] std::vector<Delivery>& getDelivery() { return deliveries;}

    // ================= Operators =================
    GameManager& operator=(const GameManager& manager) {
        if (this != &manager) {
            player = manager.player;
            foodItems = manager.foodItems;
        }
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const GameManager& manager) {
        ostream << "=== Game Manager ===\n";
        ostream << "Player: " << manager.player << " RON\n";
        ostream << "Food items:\n";
        for (const auto& food : manager.foodItems)
            ostream << "  " << food;
        return ostream;
    }
};


int main() {
    Player player("Stoicescu", 0.0);
    GameManager gameManager = GameManager::loadFromFile("resources/textfile.txt", player);

    std::vector<bool> unlocked;
    for (const auto& food : gameManager.getFoods())
        unlocked.push_back(player.getMoney() >= food.getUnlockCost());
    unlocked[0] = true;

    sf::RenderWindow window;
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const unsigned int width = desktop.size.x;
    const unsigned int height = desktop.size.y;

    window.create(sf::VideoMode({width, height}, desktop.bitsPerPixel),
        "Luca Clicker", sf::Style::Default, sf::State::Windowed);

    window.setFramerateLimit(30);
    sf::Font font;

    if (!font.openFromFile("resources/font/MightySouly-lxggD.ttf")) {
        std::cerr << "Failed to load font!\n";
        return 1;
    }

    sf::Text text(font,"");
    text.setCharacterSize(50);
    text.setFillColor(sf::Color::White);

    sf::Text warningText(font,"");
    warningText.setCharacterSize(50);
    warningText.setFillColor(sf::Color::Red);

    bool running = true;
    char lastAction = ' ';
    int selectedIndex = 1;
    std::string warningMessage;
    sf::Clock warningClock;

    while (window.isOpen() && running) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                running = false;
                window.close();
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                using Scan = sf::Keyboard::Scan;
                switch (keyPressed->scancode) {
                    case Scan::Q: running = false; break;
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

        if (lastAction != ' ') {
            if (static_cast<size_t>(selectedIndex) <= gameManager.getFoods().size()) {
                FoodItem& food = gameManager.getFoods()[selectedIndex - 1];
                if (unlocked[selectedIndex - 1]) {
                    Delivery& delivery = gameManager.getDelivery()[selectedIndex - 1];
                    switch (lastAction) {
                        case 's': GameManager::sell(food, player); break;
                        case 'u': GameManager::upgrade(food, player); break;
                        case 'd': gameManager.startDelivery(food, delivery); break;
                        default: ;
                    }
                    warningMessage.clear();
                } else {
                    warningMessage = "Cannot sell or upgrade '" + food.getfoodName() +
                                     "' (unlock cost: " + std::to_string(static_cast<int>(food.getUnlockCost())) + " RON)";
                    warningClock.restart();
                }
            }
            lastAction = ' ';
        }

        for (size_t i = 0; i < unlocked.size(); ++i)
            if (!unlocked[i] && player.getMoney() >= gameManager.getFoods()[i].getUnlockCost())
                unlocked[i] = true;

        // === Build display text ===
        std::ostringstream buffer;
        buffer << "================ Luca Clicker ========================\n";
        buffer << "Controls: [S] Sell | [U] Upgrade | [D] Delivery | [Q] Quit\n";
        buffer << "Use [1-9] to select a food item.\n";
        buffer << "======================================================\n";
        buffer << "Money: " << player.getMoney() << " RON\n";
        buffer << "Currently selected item: " << selectedIndex << "\n\n";

        for (size_t i = 0; i < gameManager.getFoods().size(); ++i) {
            const auto& food = gameManager.getFoods()[i];
            const auto& delivery = gameManager.getDelivery()[i];
            if (unlocked[i])
                buffer << "[" << i + 1 << "] " << food.getfoodName()
                       << " - Income: " << food.getBaseIncome()
                       << " | Upgrade: " << food.getUpgradeCost()
                       << " | Delivery: " << delivery.getDeliveryUnlockCost() << "\n";
            else
                buffer << "[" << i + 1 << "] (LOCKED - unlock at "
                       << food.getUnlockCost() << " RON)\n";
        }

        text.setString(buffer.str());
        text.setPosition(sf::Vector2f(20.f, 20.f));

        if (!warningMessage.empty()) {

            warningText.setString(warningMessage);
            sf::FloatRect bounds = warningText.getLocalBounds();
            float textHeight = bounds.position.y + bounds.size.y;
            float windowHeight = static_cast<float>(window.getSize().y);
            warningText.setPosition({20.f, windowHeight - textHeight - 250.f});
            warningText.setFillColor(sf::Color::Red);
        }

        // Hide after 3 s
        if (!warningMessage.empty() && warningClock.getElapsedTime().asSeconds() > 3)
            warningMessage.clear();

        window.clear(sf::Color(20, 20, 20));
        window.draw(text);
        if (!warningMessage.empty())
            window.draw(warningText);
        window.display();

        sf::sleep(sf::milliseconds(50));
    }

    gameManager.stopDelivery();
    std::cout << "Exiting game...\n";
}
