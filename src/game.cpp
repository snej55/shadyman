#include "game.hpp"
#include "constants.hpp"

#include <sstream>
#include <iostream>

// ------- Core game functions ------- //

void Game::init()
{
    // create window
    InitWindow(CST::SCR_WIDTH, CST::SCR_HEIGHT, CST::WIN_NAME);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    SetTargetFPS(60);

    // load components
    updateRenderBuffer(CST::SCR_WIDTH, CST::SCR_HEIGHT);

    m_world.loadFromFile(m_mapPath.c_str());

    std::cout << "Initialized!\n";
}

void Game::update()
{
    ClearBackground(BLACK);
    checkScreenResize();
    drawFPS();
}

void Game::run()
{
    std::cout << "Running!\n";

    while (!WindowShouldClose())
    {
        BeginTextureMode(m_targetBuffer);
        // render to screen buffer
        
        // update components and do rendering stuff
        update();
        
        // end rendering to screen buffer
        EndTextureMode();

        BeginDrawing();
        
        DrawTexturePro(m_targetBuffer.texture, 
            m_srcRect,
            m_destRect, 
            Vector2{0, 0}, 0, WHITE);  

        EndDrawing();
    }
}

void Game::close()
{
    UnloadRenderTexture(m_targetBuffer);
    CloseWindow();
    std::cout << "Closed!" << std::endl;
}

// ------- Other stuff ------- //

void Game::checkScreenResize()
{
    // if window has been resized, update buffer
    if (m_width != GetScreenWidth() || m_height != GetScreenHeight())
    {
        updateRenderBuffer(GetScreenWidth(), GetScreenHeight());
    }
    m_width = GetScreenWidth();
    m_height = GetScreenHeight();
}

void Game::updateRenderBuffer(const int width, const int height)
{
    UnloadRenderTexture(m_targetBuffer);
    m_targetBuffer = LoadRenderTexture(static_cast<float>(width) / CST::SCR_VRATIO, static_cast<float>(height) / CST::SCR_VRATIO);
    m_srcRect = {0.0f, 0.0f, static_cast<float>(m_targetBuffer.texture.width), -static_cast<float>(m_targetBuffer.texture.height)};
    m_destRect = {-CST::SCR_VRATIO, -CST::SCR_VRATIO, GetScreenWidth() + (CST::SCR_VRATIO * 2), GetScreenHeight() + (CST::SCR_VRATIO * 2)};

    std::cout << "Resized render buffer to: " << width << " * " << height << '\n';
}

void Game::drawFPS()
{
    float frameTime {GetFrameTime() * 1000.f};
    std::stringstream ss{};
    ss << "Frame time: " << frameTime << " ms";
    DrawText(ss.str().c_str(), 5, 5, 10, WHITE);
    ss.str("");
    ss << "Win. Dimensions: " << GetScreenWidth() << " * " << GetScreenHeight();
    DrawText(ss.str().c_str(), 5, 16, 10, WHITE);
}