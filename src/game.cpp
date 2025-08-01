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

    // initialize audio device
    InitAudioDevice();

    SetTargetFPS(60);

    // load components
    updateRenderBuffer(CST::SCR_WIDTH, CST::SCR_HEIGHT);

    m_world.loadFromFile(m_mapPath.c_str());
    m_assets.init();

    m_player.loadAnim(&m_assets);

    m_entityManager.init(&m_assets);
    m_entityManager.addEntity(EnemyType::BLOBBO, {50, 10}, &m_assets);

    m_blaster = new Blaster{&m_player, "default",  {0.f, 1.f}};
    m_blaster->init(&m_assets);

    m_music = LoadMusicStream("data/audio/music/groove.wav");
    if (!IsMusicValid(m_music))
    {
        std::cout << "ERROR: Failed to load music stream!\n";
    }
    PauseMusicStream(m_music);

    std::cout << "Initialized!\n";
}

bool Game::menu()
{
    PauseMusicStream(m_music);
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
            
            DrawTextEx(*m_assets.getFont("pixel"), "Press [s] to toggle settings menu", {width * 0.05f, height * 0.6f}, static_cast<int>((width * 0.5) / ((float)CST::SCR_WIDTH * 0.25f / CST::SCR_VRATIO) * 4.f), 0, WHITE);
            DrawTextEx(*m_assets.getFont("pixel"), "Press [c] to toggle controls menu", {width * 0.05f, height * 0.7f}, static_cast<int>((width * 0.5) / ((float)CST::SCR_WIDTH * 0.25f / CST::SCR_VRATIO) * 4.f), 0, WHITE);
            DrawTextEx(*m_assets.getFont("pixel"), "Press [space] to start", {width * 0.05f, height * 0.8f}, static_cast<int>((width * 0.5) / ((float)CST::SCR_WIDTH * 0.25f / CST::SCR_VRATIO) * 4.f), 0, WHITE);
            
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
        
        DrawTexturePro(m_targetBuffer.texture,
            m_srcRect,
            m_destRect,
            Vector2{0, 0}, 0, WHITE);
        DrawTextEx(*m_assets.getFont("pixel"), "A game by @snej55", {20, (float)m_height - 30}, 24, 0, WHITE);
        
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
                PlaySound(*m_assets.getSound("button"));
                showControls = !showControls;
            }
        } else if (IsKeyPressed(KEY_S))
        {
            if (!showControls)
            {
                PlaySound(*m_assets.getSound("button"));
                showSettings = !showSettings;
            }
        } else if (IsKeyPressed(KEY_SPACE))
        {
            if (!showControls && !showSettings)
            {
                PlaySound(*m_assets.getSound("button"));
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
                    PlaySound(*m_assets.getSound("button"));
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

    m_gameTime += m_dt;
    m_interval = std::max(1.f, m_interval - 0.005f * m_dt);
    m_distance = std::min(650.f, m_distance + 0.02f * m_dt);

    m_timer += m_dt;
    if (m_timer > m_interval)
    {
        m_timer = 0.0f;
        m_entityManager.addEntity(Util::random() < 0.5f ? EnemyType::BLOBBO : EnemyType::PENGUIN, {Util::random() * m_distance + 10.f, -10}, &m_assets);
        m_entityManager.addEntity(Util::random() < 0.5f ? EnemyType::BLOBBO : EnemyType::PENGUIN, {1188 - Util::random() * m_distance, -10}, &m_assets);
    }

    // -------------------------- //
    // ------ do rendering ------  //

    ClearBackground({20, 60, 108, 0xFF});
    // ClearBackground(BLACK);

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

    m_entityManager.update(m_dt, &m_world, &m_player, renderScroll, m_blaster, m_screenShake, m_coins, m_slomo);

    // -------------------------- //

    checkScreenResize();
}

void Game::run()
{
    std::cout << "Running!\n";

    double lastTime {GetTime()};

    PlayMusicStream(m_music);
    while (!WindowShouldClose())
    {
        UpdateMusicStream(m_music);
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
                        PlayMusicStream(m_music);
                        m_paused = false;
                    }
                }
            }
        }

        // end rendering to screen buffer
        EndTextureMode();

        renderLights();

        BeginDrawing();

        Texture2D* tex {m_assets.getTexture("noise")};
        BeginShaderMode(*m_assets.getShader("screenShader"));
        SetShaderValueTexture(*m_assets.getShader("screenShader"), GetShaderLocation(*m_assets.getShader("screenShader"), "lighting"), m_lightingBuffer.texture);
        SetShaderValueTexture(*m_assets.getShader("screenShader"), GetShaderLocation(*m_assets.getShader("screenShader"), "noise"), *tex);
        SetShaderValue(*m_assets.getShader("screenShader"), GetShaderLocation(*m_assets.getShader("screenShader"), "width"), &m_width, SHADER_UNIFORM_INT);
        SetShaderValue(*m_assets.getShader("screenShader"), GetShaderLocation(*m_assets.getShader("screenShader"), "height"), &m_height, SHADER_UNIFORM_INT);
        float time {static_cast<float>(GetTime())};
        SetShaderValue(*m_assets.getShader("screenShader"), GetShaderLocation(*m_assets.getShader("screenShader"), "time"), &time, SHADER_UNIFORM_FLOAT);
        SetShaderValue(*m_assets.getShader("screenShader"), GetShaderLocation(*m_assets.getShader("screenShader"), "scrollx"), &m_scroll.x, SHADER_UNIFORM_FLOAT);
        SetShaderValue(*m_assets.getShader("screenShader"), GetShaderLocation(*m_assets.getShader("screenShader"), "scrolly"), &m_scroll.y, SHADER_UNIFORM_FLOAT);
        SetShaderValue(*m_assets.getShader("screenShader"), GetShaderLocation(*m_assets.getShader("screenShader"), "darkness"), &m_darkness, SHADER_UNIFORM_FLOAT);
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
            SetMusicVolume(m_music, 0.2f);
            shop();
            m_shopFade += (1.0 - m_shopFade) * 0.25f * m_dt;
        } else {
            SetMusicVolume(m_music, m_darkness);
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
            if (m_darkness == 1.0f)
            {
                PlaySound(*m_assets.getSound("boom"));
            }
            SetMusicVolume(m_music, m_darkness);
            m_darkness -= 0.01f * m_dt;
        }
        
        if (m_darkness <= 0.0f)
        {
            PauseMusicStream(m_music);
            return;
        }
    }
}

bool Game::death()
{
    PauseMusicStream(m_music);
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
        
        DrawTexturePro(m_targetBuffer.texture,
            m_srcRect,
            m_destRect,
            Vector2{0, 0}, 0, WHITE);
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
    UnloadRenderTexture(m_lightingBuffer);
    UnloadMusicStream(m_music);
    CloseAudioDevice();
    CloseWindow();
    std::cout << "Closed!" << std::endl;
}

void Game::reset()
{
    m_player.setHealth(m_player.getMaxHealth());
    m_player.setRecovery(999.f);
    m_player.setPos(m_spawnPos);
    m_entityManager.free();
    m_entityManager.init(&m_assets);
    m_darkness = 1.0f;
    m_coins = 0.0f;
    delete m_blaster;
    m_blaster = new Blaster{&m_player, "default",  {0.f, 1.f}};
    m_blaster->init(&m_assets);
    m_currentBlaster = "Default";
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
    UnloadRenderTexture(m_lightingBuffer);
    m_lightingBuffer = LoadRenderTexture(static_cast<float>(width) / CST::SCR_VRATIO, static_cast<float>(height) / CST::SCR_VRATIO);

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
            PlaySound(*m_assets.getSound("button"));
            m_shop = true;
        }
    }
}

void Game::shop()
{
    DrawRectangle(0, 0, m_width, m_height, {21, 10, 31, static_cast<unsigned char>(static_cast<int>(m_shopFade * 255.f))});
    
    // render coin anim
    DrawTexturePro(*m_assets.getTexture("coin"), {7.f * std::floor(m_coinAnim), 0.0f, 7.f, 7.f}, {m_width * 0.5f - 10.f * CST::SCR_VRATIO, 5.f * CST::SCR_VRATIO, 7.f * CST::SCR_VRATIO, 7.f * CST::SCR_VRATIO}, {0.0f, 0.0f}, 0.0f, WHITE);
    std::stringstream ss{};
    const float coinVel = (m_coins - m_coinCounter) / 4.f * m_dt;
    m_coinCounter += coinVel;
    m_coinAnimSpeed = 0.2f + coinVel;
    ss << "x" << std::floor(m_coinCounter);
    DrawTextEx(*m_assets.getFont("pixel"), ss.str().c_str(), {m_width * 0.5f, 6.f * CST::SCR_VRATIO}, 24, 0, WHITE);
    
    constexpr float scr_width {1200.f};
    Texture2D* thumb {m_assets.getTexture("thumbnails/blaster")};
    constexpr float padding{10.f};
    constexpr float spacing{150.f};
    const float width{scr_width * 0.5f - 40.f * CST::SCR_VRATIO - padding * CST::SCR_VRATIO * 2.f};
    const float height{width / (float)thumb->width * (float)thumb->height};
    DrawRectangleRounded({20.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO, scr_width * 0.5f - 40.f * CST::SCR_VRATIO, scr_width * 0.5f - 40.f * CST::SCR_VRATIO}, 0.1f, 40.f, {157, 99, 58, 255});
    DrawTexturePro(*thumb, {0, 0, (float)thumb->width, (float)thumb->height}, {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO, width, height}, {0.0f, 0.0f}, 0.0f, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Default blaster (boring): ", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 5.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Damage: 4,", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 10.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Knockback: Weak,", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 15.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Rate: slow,", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 20.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Recoil: weak", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 25.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Bidirectional shooting", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 30.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Don't waste your money mate.", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 40.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Price: $720", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 45.f * CST::SCR_VRATIO}, 16, 0, WHITE);

    Texture2D* tex{nullptr};
    if (m_coins > 720.f)
    {
        tex = m_assets.getTexture("buy");
    } else {
        tex = m_assets.getTexture("nope");
    }
    DrawTexturePro(*tex, {0, 0, 23.f, 12.f}, {std::floor(scr_width * 0.5f - 43.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO), std::floor(scr_width * 0.5f - 28.f * CST::SCR_VRATIO), 23.f * CST::SCR_VRATIO, 12.f * CST::SCR_VRATIO}, {0.0f, 0.0f}, 0.0f, WHITE);

    Button defaultButton{{scr_width * 0.5f - 43.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, scr_width * 0.5f - 28.f * CST::SCR_VRATIO}, {static_cast<int>(23.f * CST::SCR_VRATIO), static_cast<int>(12.f * CST::SCR_VRATIO)}, m_assets.getTexture("nope")};
    defaultButton.update(1.f);
    if (defaultButton.getHover())
    {
        DrawRectangle(scr_width * 0.5f - 43.f * CST::SCR_VRATIO + CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, scr_width * 0.5f - 28.f * CST::SCR_VRATIO + CST::SCR_VRATIO, static_cast<int>(23.f * CST::SCR_VRATIO) - 2 * CST::SCR_VRATIO, static_cast<int>(12.f * CST::SCR_VRATIO) - 2 * CST::SCR_VRATIO, {255, 255, 255, 100});
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (m_coins > 720.f)
        {
            if (defaultButton.getHover())
            {
                buyBlaster(Blasters::DEFAULT);
            }
        }
    }

    m_shopScroll -= spacing;
    thumb = m_assets.getTexture("thumbnails/fire_blaster");
    DrawRectangleRounded({20.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO, scr_width * 0.5f - 40.f * CST::SCR_VRATIO, scr_width * 0.5f - 40.f * CST::SCR_VRATIO}, 0.1f, 40.f, {157, 99, 58, 255});
    DrawTexturePro(*thumb, {0, 0, (float)thumb->width, (float)thumb->height}, {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO, width, height}, {0.0f, 0.0f}, 0.0f, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Fire blaster: ", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 5.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Damage: 8,", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 10.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Knockback: Strong,", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 15.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Rate: Fast,", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 20.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Recoil: weak", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 25.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Bidirectional shooting", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 30.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Just the default: upgraded.", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 40.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Price: $1200", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 45.f * CST::SCR_VRATIO}, 16, 0, WHITE);

    if (m_coins > 1200.f)
    {
        tex = m_assets.getTexture("buy");
    } else {
        tex = m_assets.getTexture("nope");
    }
    DrawTexturePro(*tex, {0, 0, 23.f, 12.f}, {std::floor(scr_width * 0.5f - 43.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO), std::floor(scr_width * 0.5f - 28.f * CST::SCR_VRATIO), 23.f * CST::SCR_VRATIO, 12.f * CST::SCR_VRATIO}, {0.0f, 0.0f}, 0.0f, WHITE);

    Button fireButton{{scr_width * 0.5f - 43.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, scr_width * 0.5f - 28.f * CST::SCR_VRATIO}, {static_cast<int>(23.f * CST::SCR_VRATIO), static_cast<int>(12.f * CST::SCR_VRATIO)}, m_assets.getTexture("nope")};
    fireButton.update(1.f);
    if (fireButton.getHover())
    {
        DrawRectangle(scr_width * 0.5f - 43.f * CST::SCR_VRATIO + CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, scr_width * 0.5f - 28.f * CST::SCR_VRATIO + CST::SCR_VRATIO, static_cast<int>(23.f * CST::SCR_VRATIO) - 2 * CST::SCR_VRATIO, static_cast<int>(12.f * CST::SCR_VRATIO) - 2 * CST::SCR_VRATIO, {255, 255, 255, 100});
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (m_coins > 1200.f)
        {
            if (fireButton.getHover())
            {
                buyBlaster(Blasters::FIRE_BLASTER);
            }
        }
    }

    m_shopScroll -= spacing;
    thumb = m_assets.getTexture("thumbnails/cannon");
    DrawRectangleRounded({20.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO, scr_width * 0.5f - 40.f * CST::SCR_VRATIO, scr_width * 0.5f - 40.f * CST::SCR_VRATIO}, 0.1f, 40.f, {157, 99, 58, 255});
    DrawTexturePro(*thumb, {0, 0, (float)thumb->width, (float)thumb->height}, {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO, width, height}, {0.0f, 0.0f}, 0.0f, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Cannon: ", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 5.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Damage: 11,", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 10.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Knockback: Strong,", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 15.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Rate: Slow", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 20.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Recoil: strong", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 25.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Bidirectional shooting", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 30.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "An interesting cannon.", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 40.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Price: $1700", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 45.f * CST::SCR_VRATIO}, 16, 0, WHITE);

    if (m_coins > 1700.f)
    {
        tex = m_assets.getTexture("buy");
    } else {
        tex = m_assets.getTexture("nope");
    }
    DrawTexturePro(*tex, {0, 0, 23.f, 12.f}, {std::floor(scr_width * 0.5f - 43.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO), std::floor(scr_width * 0.5f - 28.f * CST::SCR_VRATIO), 23.f * CST::SCR_VRATIO, 12.f * CST::SCR_VRATIO}, {0.0f, 0.0f}, 0.0f, WHITE);

    Button cannonButton{{scr_width * 0.5f - 43.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, scr_width * 0.5f - 28.f * CST::SCR_VRATIO}, {static_cast<int>(23.f * CST::SCR_VRATIO), static_cast<int>(12.f * CST::SCR_VRATIO)}, m_assets.getTexture("nope")};
    cannonButton.update(1.f);
    if (cannonButton.getHover())
    {
        DrawRectangle(scr_width * 0.5f - 43.f * CST::SCR_VRATIO + CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, scr_width * 0.5f - 28.f * CST::SCR_VRATIO + CST::SCR_VRATIO, static_cast<int>(23.f * CST::SCR_VRATIO) - 2 * CST::SCR_VRATIO, static_cast<int>(12.f * CST::SCR_VRATIO) - 2 * CST::SCR_VRATIO, {255, 255, 255, 100});
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (m_coins > 1700.f)
        {
            if (cannonButton.getHover())
            {
                buyBlaster(Blasters::CANNON);
            }
        }
    }

    m_shopScroll -= spacing;
    thumb = m_assets.getTexture("thumbnails/big_modda");
    DrawRectangleRounded({20.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO, scr_width * 0.5f - 40.f * CST::SCR_VRATIO, scr_width * 0.5f - 40.f * CST::SCR_VRATIO}, 0.1f, 40.f, {157, 99, 58, 255});
    DrawTexturePro(*thumb, {0, 0, (float)thumb->width, (float)thumb->height}, {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO, width, height}, {0.0f, 0.0f}, 0.0f, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Big Modda: ", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 5.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Damage: 30,", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 10.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Knockback: Powerful,", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 15.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Rate: Slow", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 20.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Recoil: very strong", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 25.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Bidirectional shooting", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 30.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Overkill - have fun!.", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 40.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Price: $5000", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 45.f * CST::SCR_VRATIO}, 16, 0, WHITE);

    if (m_coins > 5000.f)
    {
        tex = m_assets.getTexture("buy");
    } else {
        tex = m_assets.getTexture("nope");
    }
    DrawTexturePro(*tex, {0, 0, 23.f, 12.f}, {std::floor(scr_width * 0.5f - 43.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO), std::floor(scr_width * 0.5f - 28.f * CST::SCR_VRATIO), 23.f * CST::SCR_VRATIO, 12.f * CST::SCR_VRATIO}, {0.0f, 0.0f}, 0.0f, WHITE);

    Button moddaButton{{scr_width * 0.5f - 43.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, scr_width * 0.5f - 28.f * CST::SCR_VRATIO}, {static_cast<int>(23.f * CST::SCR_VRATIO), static_cast<int>(12.f * CST::SCR_VRATIO)}, m_assets.getTexture("nope")};
    moddaButton.update(1.f);
    if (moddaButton.getHover())
    {
        DrawRectangle(scr_width * 0.5f - 43.f * CST::SCR_VRATIO + CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, scr_width * 0.5f - 28.f * CST::SCR_VRATIO + CST::SCR_VRATIO, static_cast<int>(23.f * CST::SCR_VRATIO) - 2 * CST::SCR_VRATIO, static_cast<int>(12.f * CST::SCR_VRATIO) - 2 * CST::SCR_VRATIO, {255, 255, 255, 100});
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (m_coins > 5000.f)
        {
            if (moddaButton.getHover())
            {
                buyBlaster(Blasters::BIG_MODDA);
            }
        }
    }

    m_shopScroll -= spacing;
    thumb = m_assets.getTexture("thumbnails/exterminator");
    DrawRectangleRounded({20.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO, scr_width * 0.5f - 40.f * CST::SCR_VRATIO, scr_width * 0.5f - 40.f * CST::SCR_VRATIO}, 0.1f, 40.f, {157, 99, 58, 255});
    DrawTexturePro(*thumb, {0, 0, (float)thumb->width, (float)thumb->height}, {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO, width, height}, {0.0f, 0.0f}, 0.0f, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Blobbo exterminator: ", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 5.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Damage: 40,", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 10.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Knockback: very strong,", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 15.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Rate: very fast", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 20.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Recoil: very weak", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 25.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Bidirectional shooting", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 30.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Strikes fear into blobbos.", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 40.f * CST::SCR_VRATIO}, 16, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Price: $10000", {20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, 20.f * CST::SCR_VRATIO + padding * CST::SCR_VRATIO + height + 45.f * CST::SCR_VRATIO}, 16, 0, WHITE);

    if (m_coins > 10000.f)
    {
        tex = m_assets.getTexture("buy");
    } else {
        tex = m_assets.getTexture("nope");
    }
    DrawTexturePro(*tex, {0, 0, 23.f, 12.f}, {std::floor(scr_width * 0.5f - 43.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO), std::floor(scr_width * 0.5f - 28.f * CST::SCR_VRATIO), 23.f * CST::SCR_VRATIO, 12.f * CST::SCR_VRATIO}, {0.0f, 0.0f}, 0.0f, WHITE);

    Button extermButton{{scr_width * 0.5f - 43.f * CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, scr_width * 0.5f - 28.f * CST::SCR_VRATIO}, {static_cast<int>(23.f * CST::SCR_VRATIO), static_cast<int>(12.f * CST::SCR_VRATIO)}, m_assets.getTexture("nope")};
    extermButton.update(1.f);
    if (extermButton.getHover())
    {
        DrawRectangle(scr_width * 0.5f - 43.f * CST::SCR_VRATIO + CST::SCR_VRATIO - m_shopScroll * CST::SCR_VRATIO, scr_width * 0.5f - 28.f * CST::SCR_VRATIO + CST::SCR_VRATIO, static_cast<int>(23.f * CST::SCR_VRATIO) - 2 * CST::SCR_VRATIO, static_cast<int>(12.f * CST::SCR_VRATIO) - 2 * CST::SCR_VRATIO, {255, 255, 255, 100});
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (m_coins > 10000.f)
        {
            if (extermButton.getHover())
            {
                buyBlaster(Blasters::EXTERMINATOR);
            }
        }
    }

    m_shopScroll += spacing * static_cast<float>(static_cast<int>(Blasters::NONE) - 1);
    
    DrawTextEx(*m_assets.getFont("pixel"), ("Current blaster: " + m_currentBlaster).c_str(), {10.f * CST::SCR_VRATIO, m_height - 21.f * CST::SCR_VRATIO}, 24, 0, WHITE);
    DrawTextEx(*m_assets.getFont("pixel"), "Press [s] to close the shop", {10.f * CST::SCR_VRATIO, m_height - 13.f * CST::SCR_VRATIO}, 24, 0, WHITE);

    if (IsKeyPressed(KEY_S))
    {
        PlaySound(*m_assets.getSound("button"));
        m_shop = false;
        m_shopScroll = 0.0f;
    }

    constexpr float scrollSpeed{-20.f};
    m_shopScroll = std::max(0.0f, std::min(spacing * static_cast<float>(static_cast<int>(Blasters::NONE) - 1), m_shopScroll + GetMouseWheelMove() * scrollSpeed));

    if (IsKeyDown(KEY_LEFT))
    {
        m_shopScroll = std::max(0.0f, std::min(spacing * static_cast<float>(static_cast<int>(Blasters::NONE) - 1), m_shopScroll + scrollSpeed * 0.5f * m_dt));
    }
    if (IsKeyDown(KEY_RIGHT))
    {
        m_shopScroll = std::max(0.0f, std::min(spacing * static_cast<float>(static_cast<int>(Blasters::NONE) - 1), m_shopScroll - scrollSpeed * 0.5f * m_dt));
    }
    
    checkScreenResize();
}

void Game::buyBlaster(Blasters blasterType)
{
    switch (blasterType)
    {
        case Blasters::DEFAULT:
            if (m_coins >= 720.f)
            {
                delete m_blaster;
                m_blaster = new Blaster{&m_player, "default",  {0.f, 1.f}};
                m_blaster->init(&m_assets);
                m_coins -= 720.f;
                m_currentBlaster = "Default";
                PlaySound(*m_assets.getSound("button"));
            } else {
                std::cout << "ERROR: Not enough coins!\n";
            }
            return;
        case Blasters::FIRE_BLASTER:
            if (m_coins >= 1200.f)
            {
                delete m_blaster;
                m_blaster = new FireBlaster{&m_player, "fire_blaster",  {0.f, 1.f}};
                m_blaster->init(&m_assets);
                m_coins -= 1200.f;
                m_currentBlaster = "Fire blaster";
                PlaySound(*m_assets.getSound("button"));
            } else {
                std::cout << "ERROR: Not enough coins!\n";
            }
            return;
        case Blasters::CANNON:
            if (m_coins >= 1700.f)
            {
                delete m_blaster;
                m_blaster = new Cannon{&m_player, "cannon",  {0.f, 1.f}};
                m_blaster->init(&m_assets);
                m_coins -= 1700.f;
                m_currentBlaster = "Cannon";
                PlaySound(*m_assets.getSound("button"));
            } else {
                std::cout << "ERROR: Not enough coins!\n";
            }
            return;
        case Blasters::BIG_MODDA:
            if (m_coins >= 5000.f)
            {
                delete m_blaster;
                m_blaster = new BigModda{&m_player, "big_modda",  {0.f, 1.f}};
                m_blaster->init(&m_assets);
                m_coins -= 5000.f;
                m_currentBlaster = "Big Modda";
                PlaySound(*m_assets.getSound("button"));
            } else {
                std::cout << "ERROR: Not enough coins!\n";
            }
            return;
        case Blasters::EXTERMINATOR:
            if (m_coins >= 10000.f)
            {
                delete m_blaster;
                m_blaster = new Exterminator{&m_player, "exterminator",  {0.f, 1.f}};
                m_blaster->init(&m_assets);
                m_coins -= 10000.f;
                m_currentBlaster = "Exterminator";
                PlaySound(*m_assets.getSound("button"));
            } else {
                std::cout << "ERROR: Not enough coins!\n";
            }
            return;
        default:
            std::cout << "You have " << m_coins << " coins!\n";
    }
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

    if (IsKeyDown(KEY_X) || IsKeyDown(KEY_SPACE))
    {
        m_blaster->fire();
    }


    if (IsKeyPressed(KEY_P))
    {
        if (m_paused == false)
        {
            PauseMusicStream(m_music);
            m_paused = true;
            update();
        }
    }

    m_player.getController()->setControl(C_RIGHT, IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D));
    m_player.getController()->setControl(C_LEFT, IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A));
    m_player.getController()->setControl(C_DOWN, IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S));
}

void Game::renderLights()
{
    BeginTextureMode(m_lightingBuffer);

    ClearBackground(BLACK);

    Texture2D* tex {m_assets.getTexture("light")};
    DrawTexturePro(*tex, {0, 0, static_cast<float>(tex->width), static_cast<float>(tex->height)}, {m_player.getCenter().x - 100.f - m_scroll.x, m_player.getCenter().y - 100.f - m_scroll.y, 200.f, 200.f}, {0.0f, 0.0f}, 0.0f, WHITE);

    m_entityManager.renderLighting({static_cast<int>(m_scroll.x), static_cast<int>(m_scroll.y)});

    EndTextureMode();
}