#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#include "constants.hpp"
#include "tiles.hpp"
#include "assets.hpp"

#include <string>

class Game
{
public:
    Game() = default;
    ~Game() = default;

    // core functions
    void init();

    // update components
    void update();

    void run();

    void close();
    
    // other stuff

    // check if screen has been resized
    void checkScreenResize();
    // update render buffer
    void updateRenderBuffer(int width, int height);

    void drawFPS();

    [[nodiscard]] World* getWorld() {return &m_world;}
    [[nodiscard]] AssetManager* getAssets() {return &m_assets;}


private:
    // render buffer
    RenderTexture2D m_targetBuffer{};
    Rectangle m_srcRect{};
    Rectangle m_destRect{};

    // components
    World m_world{};
    AssetManager m_assets{};

    std::string m_mapPath{"data/maps/0.json"};

    int m_width{};
    int m_height{};

    vec2<float> m_scroll{0.0f, 0.0f};
};

#endif