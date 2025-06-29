#include "game.hpp"
#include "constants.hpp"

#include <sstream>

void Game::init()
{
    InitWindow(CST::SCR_WIDTH, CST::SCR_HEIGHT, CST::WIN_NAME.c_str());
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    SetTargetFPS(60);
}

void Game::run()
{
    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground({0x00, 0x00, 0x00});

        float frameTime {GetFrameTime() * 1000.f};
        std::stringstream ss{};
        ss << "Frame time: " << frameTime << " ms";
        DrawText(ss.str().c_str(), 5, 5, 10, WHITE);

        EndDrawing();
    }
}

void Game::close()
{
    CloseWindow();
}