#include "src/game.hpp"

#define PLATFORM_WEB
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

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