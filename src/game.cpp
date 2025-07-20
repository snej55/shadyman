#include "game.hpp"
#include "constants.hpp"
#include "util.hpp"
#include "buttons.hpp"

#include <raylib.h>
#include <sstream>
#include <iostream>

// ------- Core game functions ------- //

void Game::init()
{
    // create window
    InitWindow(CST::SCR_WIDTH, CST::SCR_HEIGHT, CST::WIN_NAME);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(CST::SCR_WIDTH, CST::SCR_HEIGHT);

    SetTargetFPS(60);

    // load components
    updateRenderBuffer(CST::SCR_WIDTH, CST::SCR_HEIGHT);

    m_world.loadFromFile(m_mapPath.c_str());
    m_assets.init();

    m_player.loadAnim(&m_assets);

    m_entityManager.init(&m_assets);
    m_entityManager.addEntity(EnemyType::BLOBBO, {50, 10}, &m_assets);

    m_blaster = new Cannon{&m_player, "default",  {0.f, 1.f}};
    m_blaster->init(&m_assets);

    std::cout << "Initialized!\n";
}

bool Game::menu()
{
    bool showControls{false};
    float controlsFade{0.0f};
    bool showSettings{false};
    float settingsFade{0.0f};

    Tick screenShakeTick{{150.f, 5.f}};
    Scale scaleSelect{{100.f, 22.f}};

    double lastTime {GetTime()};
    while (!WindowShouldClose())
    {
        BeginTextureMode(m_targetBuffer);
        // render to screen buffer

        ClearBackground(BLACK);

        if (!IsWindowResized())
        {
            // Draw UI
            const float width {static_cast<float>(m_width) / CST::SCR_VRATIO};
            const float height {static_cast<float>(m_height) / CST::SCR_VRATIO};
    
            const float padding {10.f};
            DrawRectangleRounded({width * 0.25f - padding, height * 0.1f - padding, width * 0.5f + padding * 2.f, height * 0.4f + padding * 2.f}, 0.1f, 30, GRAY);
            DrawTextEx(*m_assets.getFont("pixel"), "Shady Man", {width * 0.25f, height * 0.1f}, static_cast<int>((width * 0.5) / ((float)CST::SCR_WIDTH * 0.25f / CST::SCR_VRATIO) * 8.f), 0, WHITE);
        
            DrawTextEx(*m_assets.getFont("pixel"), "Press [s] to toggle settings", {width * 0.1f, height * 0.6f}, static_cast<int>((width * 0.5) / ((float)CST::SCR_WIDTH * 0.25f / CST::SCR_VRATIO) * 4.f), 0, WHITE);
            DrawTextEx(*m_assets.getFont("pixel"), "Press [c] to toggle controls", {width * 0.1f, height * 0.7f}, static_cast<int>((width * 0.5) / ((float)CST::SCR_WIDTH * 0.25f / CST::SCR_VRATIO) * 4.f), 0, WHITE);
            DrawTextEx(*m_assets.getFont("pixel"), "Press [space] to start", {width * 0.1f, height * 0.8f}, static_cast<int>((width * 0.5) / ((float)CST::SCR_WIDTH * 0.25f / CST::SCR_VRATIO) * 4.f), 0, WHITE);
        
            if (showControls)
            {
                controlsFade += (1.0 - controlsFade) * 0.25 * m_dt;
            } else {
                controlsFade += (0.0 - controlsFade) * 0.25 * m_dt;
            }

            DrawRectangle(0, 0, width, height, {41, 25, 69, static_cast<unsigned char>(static_cast<int>(255.f * controlsFade))});
            Texture2D* controlsTex{m_assets.getTexture("controls")};
            DrawTexturePro(*controlsTex, 
                {0, 0, static_cast<float>(controlsTex->width), static_cast<float>(controlsTex->height)},
                {std::floor(width * 0.5f - static_cast<float>(controlsTex->width) * 0.5f), std::floor(height * 0.5f - static_cast<float>(controlsTex->height) * 0.5f - height * (1.f - controlsFade)), static_cast<float>(controlsTex->width), static_cast<float>(controlsTex->height)},
                {0.0f, 0.0f},
                0.0f,
                WHITE
            );
            DrawTextEx(*m_assets.getFont("pixel"), "Press [c] to exit controls menu", {10.f, height - 20.f}, 8, 0, {255, 255, 255, static_cast<unsigned char>(static_cast<int>(255.f * controlsFade))});

            if (showSettings)
            {
                settingsFade += (1.0 - settingsFade) * 0.25 * m_dt;
            } else {
                settingsFade += (0.0 - settingsFade) * 0.25 * m_dt;
            }

            DrawRectangle(0, 0, width, height, {27, 24, 83, static_cast<unsigned char>(static_cast<int>(255.f * settingsFade))});
            DrawTextEx(*m_assets.getFont("pixel"), "Screenshake enabled: ", {10.f, 10.f}, 8, 0, {255, 255, 255, static_cast<unsigned char>(static_cast<int>(255.f * settingsFade))});
            screenShakeTick.update(CST::SCR_VRATIO);
            scaleSelect.update(CST::SCR_VRATIO);
            screenShakeTick.render(&m_assets, {0, static_cast<int>(height * (1.0 - settingsFade))});
            DrawTextEx(*m_assets.getFont("pixel"), "Screen scale: ", {10.f, 25.f}, 8, 0, {255, 255, 255, static_cast<unsigned char>(static_cast<int>(255.f * settingsFade))});
            scaleSelect.render(&m_assets, {0, static_cast<int>(height * (1.0 - settingsFade))});
            DrawTextEx(*m_assets.getFont("pixel"), "Press [s] to exit settings menu", {10.f, height - 20.f}, 8, 0, {255, 255, 255, static_cast<unsigned char>(static_cast<int>(255.f * settingsFade))});
        }
        
        // end rendering to screen buffer
        EndTextureMode();
        
        BeginDrawing();
        
        BeginShaderMode(*m_assets.getShader("screenShader"));
        DrawTexturePro(m_targetBuffer.texture,
            m_srcRect,
            m_destRect,
            Vector2{0, 0}, 0, WHITE);
        EndShaderMode();
        BeginDrawing();
        
        EndDrawing();
        
        // calculate deltatime
        m_dt = GetTime() - lastTime;
        m_dt *= 60.f * m_slomo;
        m_dt = std::min(4.0f, m_dt);
        lastTime = GetTime();
        
        // handle controls
        if (IsKeyPressed(KEY_C))
        {
            if (!showSettings)
            {
                showControls = !showControls;
            }
        } else if (IsKeyPressed(KEY_S))
        {
            if (!showControls)
            {
                showSettings = !showSettings;
            }
        } else if (IsKeyPressed(KEY_SPACE))
        {
            if (!showControls && !showSettings)
            {
                m_screenShakeEnabled = screenShakeTick.getSelected();
                CST::SCR_VRATIO = scaleSelect.getScale();
                std::cout << scaleSelect.getScale() << " " << CST::SCR_VRATIO << '\n';
                updateRenderBuffer(GetScreenWidth(), GetScreenHeight());
                m_lastPaused = 0.0f;
                m_paused = false;
                return false;
            }
        }
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (showSettings)
            {
                if (screenShakeTick.getHover())
                {
                    screenShakeTick.setSelected(!screenShakeTick.getSelected());
                }
                scaleSelect.click();
            }
        }
        checkScreenResize();
    }
    return true;
}

void Game::update()
{
    // ------ Update stuff ------ //

    m_player.update(m_dt, &m_world);
    m_blaster->update(m_dt, &m_world);

    m_timer += m_dt;
    if (m_timer > 30.f)
    {
        m_timer = 0.0f;
        m_entityManager.addEntity(EnemyType::BLOBBO, {Util::random() * 200.f + 10.f, 10}, &m_assets);
    }

    // -------------------------- //
    // ------ do rendering ------  //

    ClearBackground({30, 70, 118, 0xFF});

    m_scroll.x += std::floor((m_player.getPos().x - static_cast<float>(m_width) / CST::SCR_VRATIO / 2.f - m_scroll.x) / 6) * m_dt;
    m_scroll.y += std::floor((m_player.getPos().y - static_cast<float>(m_height) / CST::SCR_VRATIO / 2.f - m_scroll.y) / 10) * m_dt;

    m_scroll.x = std::max(static_cast<float>(CST::TILE_SIZE), std::min(m_scroll.x, static_cast<float>(CST::TILE_SIZE * CST::CHUNK_SIZE * CST::LEVEL_WIDTH)));
    m_scroll.y = std::max(0.0f, std::min(m_scroll.y, static_cast<float>(CST::TILE_SIZE * CST::LEVEL_WIDTH * CST::LEVEL_HEIGHT)));

    vec2<float> screenShakeOffset{Util::random() * m_screenShake - m_screenShake / 2.f, Util::random() * m_screenShake - m_screenShake / 2.f};
    if (!m_screenShakeEnabled)
    {
        screenShakeOffset = {0.0f, 0.0f};
    }
    constexpr float screenShakeScale {0.5f};
    vec2<int> renderScroll {static_cast<int>(m_scroll.x + screenShakeOffset.x * screenShakeScale), static_cast<int>(m_scroll.y + screenShakeOffset.y * screenShakeScale)};
    m_screenShake = std::max(0.0f, m_screenShake - m_dt);
    m_world.render(renderScroll, m_width, m_height, &m_assets);
    DrawRectangle(0, 0, m_width, m_height, {180, 35, 19, static_cast<unsigned char>(static_cast<int>((1.f - std::min(1.f, m_player.getRecovery() / m_player.getRecoverTime())) * 100.f))});

    m_player.draw(renderScroll);

    if (m_player.getRecovery() > m_player.getRecoverTime() + 20.f)
    {
        m_blaster->render(renderScroll);
    }
    m_blaster->renderBullets(renderScroll);

    m_entityManager.update(m_dt, &m_world, &m_player, renderScroll, m_blaster, m_screenShake, m_coins);

    // -------------------------- //

    checkScreenResize();
}

void Game::run()
{
    std::cout << "Running!\n";

    double lastTime {GetTime()};

    while (!WindowShouldClose())
    {
        m_lastPaused += m_dt;
        m_coinAnim += m_coinAnimSpeed * m_dt;
        if (m_coinAnim >= 6.f)
        {
            m_coinAnim = 0.0f;
        }
        BeginTextureMode(m_targetBuffer);
        // render to screen buffer

        if (!m_paused && !m_shop)
        {
            if (!IsWindowResized())
            {
                // update components and do rendering stuff
                update();
                if (m_lastPaused < 60.f)
                {
                    Texture2D* playTex {m_assets.getTexture("pause")};
                    DrawTexture(*playTex, static_cast<int>(static_cast<float>(m_width) / CST::SCR_VRATIO / 2.f - (float)playTex->width * 0.5f), static_cast<int>(static_cast<float>(m_height) / CST::SCR_VRATIO / 2.f - (float)playTex->height * 0.5f), WHITE);
                }
            } else {
                checkScreenResize();
            }
            handleControls();
        } else {
            if (!m_shop)
            {
                if (IsWindowResized())
                {
                    update();
                }
                m_lastPaused = 0.0f;
                Texture2D* playTex {m_assets.getTexture("play")};
                DrawTexture(*playTex, static_cast<int>(static_cast<float>(m_width) / CST::SCR_VRATIO / 2.f - (float)playTex->width * 0.5f), static_cast<int>(static_cast<float>(m_height) / CST::SCR_VRATIO / 2.f - (float)playTex->height * 0.5f), WHITE);
                if (IsKeyPressed(KEY_P))
                {
                    if (m_paused)
                    {
                        m_paused = false;
                    }
                }
            }
        }

        // end rendering to screen buffer
        EndTextureMode();

        BeginDrawing();

        BeginShaderMode(*m_assets.getShader("screenShader"));
        DrawTexturePro(m_targetBuffer.texture,
            m_srcRect,
            m_destRect,
            Vector2{0, 0}, 0, WHITE);
        EndShaderMode();

#ifdef DEBUG_INFO_ENABLED
        drawFPS();
#endif

        if (!m_shop)
            drawUI();

        if (m_shop)
        {
            shop();
            m_shopFade += (1.0 - m_shopFade) * 0.25f * m_dt;
        } else {
            m_shopFade += (0.0 - m_shopFade) * 0.25f * m_dt;
        }

        EndDrawing();

        // calculate deltatime
        m_dt = GetTime() - lastTime;
        m_dt *= 60.f * m_slomo;
        m_dt = std::min(4.0f, m_dt);
        lastTime = GetTime();

        // update slomo
        m_slomo += (1.0f - m_slomo) * 0.05f * (m_dt / m_slomo);

        // check if player died
        if (m_player.getHealth() <= 0.0f)
        {
            return;
        }
    }
}

bool Game::death()
{
    double lastTime {GetTime()};
    while (!WindowShouldClose())
    {
        BeginTextureMode(m_targetBuffer);
        // render to screen buffer

        ClearBackground(BLACK);

        if (!IsWindowResized())
        {
            // Draw UI
            const float width {static_cast<float>(m_width) / CST::SCR_VRATIO};
            const float height {static_cast<float>(m_height) / CST::SCR_VRATIO};
    
            const float padding {10.f};
            DrawRectangleRounded({width * 0.25f - padding, height * 0.1f - padding, width * 0.5f + padding * 2.f, height * 0.4f + padding * 2.f}, 0.1f, 30, GRAY);
            DrawTextEx(*m_assets.getFont("pixel"), "Game Over", {width * 0.25f, height * 0.1f}, static_cast<int>((width * 0.5) / ((float)CST::SCR_WIDTH * 0.25f / CST::SCR_VRATIO) * 8.f), 0, WHITE);
        
            DrawTextEx(*m_assets.getFont("pixel"), "You died.", {width * 0.1f, height * 0.6f}, static_cast<int>((width * 0.5) / ((float)CST::SCR_WIDTH * 0.25f / CST::SCR_VRATIO) * 4.f), 0, WHITE);
            DrawTextEx(*m_assets.getFont("pixel"), "Press [space] to return to menu", {width * 0.1f, height * 0.7f}, static_cast<int>((width * 0.5) / ((float)CST::SCR_WIDTH * 0.25f / CST::SCR_VRATIO) * 4.f), 0, WHITE);
            DrawTextEx(*m_assets.getFont("pixel"), "Or press [ESC] to exit the game.", {width * 0.1f, height * 0.8f}, static_cast<int>((width * 0.5) / ((float)CST::SCR_WIDTH * 0.25f / CST::SCR_VRATIO) * 4.f), 0, WHITE);
        }
        
        // end rendering to screen buffer
        EndTextureMode();
        
        BeginDrawing();
        
        BeginShaderMode(*m_assets.getShader("screenShader"));
        DrawTexturePro(m_targetBuffer.texture,
            m_srcRect,
            m_destRect,
            Vector2{0, 0}, 0, WHITE);
        EndShaderMode();
        BeginDrawing();
        
        EndDrawing();
        checkScreenResize();

        if (IsKeyPressed(KEY_SPACE))
        {
            return false;
        }
    }
    return true;
}

void Game::close()
{
    delete m_blaster;
    UnloadRenderTexture(m_targetBuffer);
    CloseWindow();
    std::cout << "Closed!" << std::endl;
}

void Game::reset()
{
    m_player.setHealth(m_player.getMaxHealth());
    m_player.setRecovery(999.f);
    m_entityManager.free();
    m_entityManager.init(&m_assets);
}

// ------- Other stuff ------- //

void Game::checkScreenResize()
{
    // if window has been resized, update buffer
    if (m_width != GetScreenWidth() || m_height != GetScreenHeight())
    {
        std::cout << "Resizing render buffer...\n";
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
    ss << "FPS: " << GetFPS() << "";

    DrawTextEx(*m_assets.getFont("pixel"), ss.str().c_str(), {5, 5}, 20, 0, WHITE);
    // DrawText(ss.str().c_str(), 5, 5, 20, WHITE);
}

void Game::drawUI()
{
    // draw bottom bar
    DrawLineEx({0, m_height - 20 * CST::SCR_VRATIO}, {static_cast<float>(m_width), m_height - 20 * CST::SCR_VRATIO}, CST::SCR_VRATIO * 2.f, {255, 253, 240, 255});
    DrawRectangle(0, m_height - 20 * CST::SCR_VRATIO, m_width, 20 * CST::SCR_VRATIO, {12, 19, 39, 200});

    // draw player health bar
    m_playerHealth += (m_player.getHealth() - m_playerHealth) / 3.0f * m_dt; // update rendered health

    // draw actual health
    constexpr float healthBarWidth {104.f};
    DrawRectangle(static_cast<int>((float)m_width / 2.f - healthBarWidth * CST::SCR_VRATIO * 0.5f), static_cast<int>(6 * CST::SCR_VRATIO), static_cast<int>(healthBarWidth * CST::SCR_VRATIO), static_cast<int>(8.f * CST::SCR_VRATIO),
        {21, 10, 31, 255}
    );
    DrawRectangle(static_cast<int>((float)m_width / 2.f - healthBarWidth * CST::SCR_VRATIO * 0.5f), static_cast<int>(6 * CST::SCR_VRATIO), static_cast<int>(m_playerHealth / m_player.getMaxHealth() * healthBarWidth * CST::SCR_VRATIO), static_cast<int>(4.f * CST::SCR_VRATIO),
        ColorLerp(Color{180, 35, 19, 255}, Color{87, 197, 43, 255}, m_playerHealth / m_player.getMaxHealth())
    );
    DrawRectangle(static_cast<int>((float)m_width / 2.f - healthBarWidth * CST::SCR_VRATIO * 0.5f), static_cast<int>(10 * CST::SCR_VRATIO), static_cast<int>(m_playerHealth / m_player.getMaxHealth() * healthBarWidth * CST::SCR_VRATIO), static_cast<int>(4.f * CST::SCR_VRATIO),
        ColorLerp(Color{104, 24, 36, 255}, Color{17, 131, 55, 255}, m_playerHealth / m_player.getMaxHealth())
    );

    // draw second layer
    Texture2D* healthBarTex = m_assets.getTexture("health_bar");
    DrawTexturePro(*healthBarTex, Rectangle{0, 0, (float)healthBarTex->width, (float)healthBarTex->height},
        {(float)m_width / 2.f - (float)healthBarTex->width * CST::SCR_VRATIO * 0.5f, 4 * CST::SCR_VRATIO, (float)healthBarTex->width * CST::SCR_VRATIO, (float)healthBarTex->height * CST::SCR_VRATIO},
        {0.0f, 0.0f}, 0.0f, WHITE
    );

    Texture2D* shopTex {m_assets.getTexture("shop")};
    DrawTexturePro(*shopTex, Rectangle{0, 0, 23, 12}, {4.f * CST::SCR_VRATIO, m_height - 16.f * CST::SCR_VRATIO, 23.f * CST::SCR_VRATIO, 12.f * CST::SCR_VRATIO}, {0, 0}, 0, WHITE);

    Button shopButton{{4.f * CST::SCR_VRATIO, m_height - 16.f * CST::SCR_VRATIO}, {static_cast<int>(23 * CST::SCR_VRATIO), static_cast<int>(12 * CST::SCR_VRATIO)}, m_assets.getTexture("shop")};
    shopButton.update(1.f);
    if (shopButton.getHover())
    {
        DrawRectangle(4.f * CST::SCR_VRATIO, m_height - 16.f * CST::SCR_VRATIO, 23.f * CST::SCR_VRATIO, 12.f * CST::SCR_VRATIO, {255, 255, 255, 100});
    }

    // render coin anim
    DrawTexturePro(*m_assets.getTexture("coin"), {7.f * std::floor(m_coinAnim), 0.0f, 7.f, 7.f}, {m_width - 45.f * CST::SCR_VRATIO, m_height - 14.f * CST::SCR_VRATIO, 7.f * CST::SCR_VRATIO, 7.f * CST::SCR_VRATIO}, {0.0f, 0.0f}, 0.0f, WHITE);
    std::stringstream ss{};
    const float coinVel = (m_coins - m_coinCounter) / 4.f * m_dt;
    m_coinCounter += coinVel;
    m_coinAnimSpeed = 0.2f + coinVel;
    ss << "x" << std::floor(m_coinCounter);
    DrawTextEx(*m_assets.getFont("pixel"), ss.str().c_str(), {m_width - 38.f * CST::SCR_VRATIO, m_height - 13.f * CST::SCR_VRATIO}, 24, 0, WHITE);

    // hurt flash
    DrawRectangle(-(m_player.getRecovery() - m_player.getRecoverTime()) - 25, 0, 50, m_height, {180, 35, 19, 150});
    DrawRectangle(m_width + (m_player.getRecovery() - m_player.getRecoverTime()) - 25, 0, 50, m_height, {180, 35, 19, 150});

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (shopButton.getHover())
        {
            m_shop = true;
        }
    }
}

void Game::shop()
{
    DrawRectangle(0, 0, m_width, m_height, {21, 10, 31, static_cast<unsigned char>(static_cast<int>(m_shopFade * 255.f))});
    DrawTextEx(*m_assets.getFont("pixel"), "Press [s] to close the shop", {10.f * CST::SCR_VRATIO, m_height - 13.f * CST::SCR_VRATIO}, 24, 0, WHITE);
    
    // render coin anim
    DrawTexturePro(*m_assets.getTexture("coin"), {7.f * std::floor(m_coinAnim), 0.0f, 7.f, 7.f}, {m_width * 0.5f - 10.f * CST::SCR_VRATIO, 5.f * CST::SCR_VRATIO, 7.f * CST::SCR_VRATIO, 7.f * CST::SCR_VRATIO}, {0.0f, 0.0f}, 0.0f, WHITE);
    std::stringstream ss{};
    const float coinVel = (m_coins - m_coinCounter) / 4.f * m_dt;
    m_coinCounter += coinVel;
    m_coinAnimSpeed = 0.2f + coinVel;
    ss << "x" << std::floor(m_coinCounter);
    DrawTextEx(*m_assets.getFont("pixel"), ss.str().c_str(), {m_width * 0.5f, 6.f * CST::SCR_VRATIO}, 24, 0, WHITE);

    if (IsKeyPressed(KEY_S))
    {
        m_shop = false;
    }

    checkScreenResize();
}

void Game::handleControls()
{
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
    {
        m_player.getController()->setControl(C_UP, true);
        m_player.jump();
    } else if (IsKeyReleased(KEY_UP) || IsKeyReleased(KEY_W))
    {
        m_player.getController()->setControl(C_UP, false);
    }

    if (m_player.getRecovery() > m_player.getRecoverTime() + 20.f)
    {
        if (IsKeyDown(KEY_X) || IsKeyDown(KEY_SPACE))
        {
            m_blaster->fire();
        }
    }

    if (IsKeyPressed(KEY_P))
    {
        if (m_paused == false)
        {
            m_paused = true;
            update();
        }
    }

    m_player.getController()->setControl(C_RIGHT, IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D));
    m_player.getController()->setControl(C_LEFT, IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A));
    m_player.getController()->setControl(C_DOWN, IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S));
}
