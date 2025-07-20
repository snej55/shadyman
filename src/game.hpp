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

    // menu
    bool menu();

    // update components
    void update();

    void run();

    // death screen
    bool death();

    void close();

    // reset everything
    void reset();

    // other stuff

    // check if screen has been resized
    void checkScreenResize();
    // update render buffer
    void updateRenderBuffer(int width, int height);
    // render debug info
    void drawFPS();
    // render ui
    void drawUI();

    void shop();

    void buyBlaster(Blasters blasterType);

    // handle user input
    void handleControls();

    void renderLights();


    [[nodiscard]] World* getWorld() {return &m_world;}
    [[nodiscard]] AssetManager* getAssets() {return &m_assets;}
    [[nodiscard]] Player* getPlayer() {return &m_player;}

    [[nodiscard]] float getDT() const {return m_dt;}
    void setSlomo(const float val) {m_slomo = val;}
    [[nodiscard]] float getSlomo() const {return m_slomo;}

private:
    // render buffer
    RenderTexture2D m_targetBuffer{};
    RenderTexture2D m_lightingBuffer{};
    Rectangle m_srcRect{};
    Rectangle m_destRect{};

    // components
    World m_world{};
    AssetManager m_assets{};
    EntityManager m_entityManager{};
    Player m_player{{20.f, 20.f}, {7, 14}};
    Blaster* m_blaster{nullptr};
    std::string m_currentBlaster{"default"};

    // random stuff
    std::string m_mapPath{"data/maps/0.json"};

    // rendering + core
    int m_width{};
    int m_height{};

    vec2<float> m_scroll{0.0f, 0.0f};
    float m_screenShake{0.0f};
    bool m_screenShakeEnabled{true};

    // deltatime
    float m_dt{1.0f};
    float m_timer{0.0f};
    float m_slomo{1.0f};

    bool m_paused{false};
    float m_lastPaused{0.0f};

    bool m_shop{false};
    float m_shopFade{0.0f};
    float m_shopScroll{0.0f};

    // UI stuff
    float m_playerHealth{0.0f};

    float m_coins{0.0f};
    float m_coinAnim{0.0f};
    float m_coinAnimSpeed{0.2f};
    float m_coinCounter{0.0f};
    float m_darkness{1.0f};
};

#endif
