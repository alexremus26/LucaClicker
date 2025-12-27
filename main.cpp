#include <iostream>

#include "src/Player.h"
#include "src/Game.h"
#include "src/Display.h"
#include "src/GameExceptions.h"

int main()
{
    try {

        Player player("Stoicescu", 100000);

        Game gameManager = Game::loadFromFile("data/load.txt", player);

        Display display(gameManager);
        display.run();
    }
    catch (const GameException& e) {
        std::cerr << "Game error: " << e.what() << '\n';
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
