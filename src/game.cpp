#include "game.hpp"
#include "constants.hpp"

void Game::init()
{
    InitWindow(CST::SCR_WIDTH, CST::SCR_HEIGHT, CST::WIN_NAME.c_str());
    SetWindowState(FLAG_WINDOW_RESIZABLE);
}

void Game::run()
{
    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground({0x00, 0x00, 0x00});

        EndDrawing();
    }
}

void Game::close()
{
    CloseWindow();
}