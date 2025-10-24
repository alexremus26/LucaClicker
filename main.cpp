#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>


class FoodItem {
    std::string foodName;
    double baseIncome;
    double upgradeCost;
    double incomeMultiplier;
    double upgradeMultiplier;

public:
    FoodItem(std::string foodName_, double baseIncome_,
             double upgradeCost_, double incomeMultiplier_, double upgradeMultiplier_)
        : foodName(foodName_), baseIncome(baseIncome_),
          upgradeCost(upgradeCost_), incomeMultiplier(incomeMultiplier_),
          upgradeMultiplier(upgradeMultiplier_) {}

    double getBaseIncome() { return baseIncome; }
    double getUpgradeCost() { return upgradeCost; }
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
    explicit Player(double money_) : money(money_) {}
    double getMoney() const { return money; }
    void setMoney(double money_) { money = money_; }
};

class GameManager {
public:
    void buy(FoodItem& obiect, Player& player) {
        double newMoney = player.getMoney() + obiect.getBaseIncome();
        player.setMoney(newMoney);
    }

    void upgrade(FoodItem& obiect, Player& player) {
        if (player.getMoney() >= obiect.getUpgradeCost()) {
            player.setMoney(player.getMoney() - obiect.getUpgradeCost());
            obiect.upgrade();
        }
    }
};


int main() {
    Player player(100.0);
    FoodItem croissant("Croissant", 5.0, 20.0, 1.0, 1.2);
    GameManager manager;

    std::cout << "Player starts with: " << player.getMoney() << "\n";

    manager.buy(croissant, player);
    std::cout << "After buy: " << player.getMoney() << "\n";

    manager.upgrade(croissant, player);
    std::cout << "After upgrade: " << player.getMoney() << "\n";
}
