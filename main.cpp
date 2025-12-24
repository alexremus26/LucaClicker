#include <string>
#include <iostream>
#include <fstream>

#include "SFML/Audio/Sound.hpp"
#include "src/Player.h"
#include "src/Game.h"
#include "src/Display.h"
#include "src/GameExceptions.h"
#include "src/ResourceManager.h"

int main() {
    try {
        Player player("Stoicescu", 100000);

        std::ifstream saveFile("../data/savefile.txt");
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
                std::remove("../data/savefile.txt");
                std::cout << "Starting new game...\n";
                saveExists = false;
            }
        }

        Game gameManager(Game::loadFromFile("../data/load.txt", player));

        if (saveExists) {
            (void)gameManager.loadSavedGame();
        }

        Display game(gameManager);
        game.run();
    }
    catch (const GameException& e) {
        std::cerr << "Game error: " << e.what() << std::endl;
        return 1;
    }
}