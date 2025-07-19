#include "src/game.hpp"

int main()
{
    Game game{};
    
    game.init();

    bool exit{false};
    while (!exit)
    {
        exit = game.menu();
        game.run();
        exit = game.death();
        game.reset();
    }

    game.close();

    return 0;
}