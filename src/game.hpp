#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#include "tiles.hpp"
#include "assets.hpp"
#include "player.hpp"
#include "entities.hpp"
#include "blasters.hpp"

#include <string>

#define DEBUG_INFO_ENABLED

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
    // render debug info
    void drawFPS();
    // render ui
    void drawUI();

    // handle user input
    void handleControls();

    [[nodiscard]] World* getWorld() {return &m_world;}
    [[nodiscard]] AssetManager* getAssets() {return &m_assets;}
    [[nodiscard]] Player* getPlayer() {return &m_player;}

    [[nodiscard]] float getDT() const {return m_dt;}

private:
    // render buffer
    RenderTexture2D m_targetBuffer{};
    Rectangle m_srcRect{};
    Rectangle m_destRect{};

    // components
    World m_world{};
    AssetManager m_assets{};
    EntityManager m_entityManager{};
    Player m_player{{20.f, 20.f}, {7, 14}};
    Blaster* m_blaster{nullptr};

    // random stuff
    std::string m_mapPath{"data/maps/0.json"};

    // rendering + core
    int m_width{};
    int m_height{};

    vec2<float> m_scroll{0.0f, 0.0f};

    // deltatime
    float m_dt{1.0f};
    float m_timer{0.0f};

    // UI stuff
    float m_playerHealth{0.0f};
};

#endif
