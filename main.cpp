#include <string>
#include <iostream>
#include <fstream>

#include "src/Player.h"
#include "src/GameManager.h"
#include "src/Display.h"

int main() {
    try {
        Player player("Stoicescu", 0.0);

        std::ifstream saveFile("resources/savegame.txt");
        bool saveExists = saveFile.good();
        saveFile.close();

        if (saveExists) {
            std::cout << "Save file found!\n";
            std::cout << "1. Continue saved game\n";
            std::cout << "2. Start new game (reset progress)\n";
            std::cout << "Choose option (1 or 2): ";

            char choice;
            std::cin >> choice;

            if (choice == '2') {
                std::remove("resources/savegame.txt");
                std::cout << "Starting new game...\n";
                saveExists = false;
            }
        }

        GameManager gameManager = GameManager::loadFromFile("resources/textfile.txt", player);

        if (saveExists) {
            (void)gameManager.loadSavedGame(); // void for the warning
        }

        Display display(gameManager, player);
        display.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}