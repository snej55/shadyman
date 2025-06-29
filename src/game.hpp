#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#include "constants.hpp"

class Game
{
public:
    Game() = default;
    ~Game() = default;

    void init();

    void run();

    void close();
};

#endif