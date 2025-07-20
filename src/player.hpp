#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

#include "vec2.hpp"
#include "tiles.hpp"
#include "assets.hpp"
#include "anim.hpp"

#include <map>

enum Control
{
    C_UP,
    C_DOWN,
    C_LEFT,
    C_RIGHT
};

class Controller
{
public:
    Controller() = default;

    void setControl(const Control control, const bool val) {m_controls[control] = val;}
    [[nodiscard]] bool getControl(const Control control) {return m_controls[control];}

private:
    std::map<Control, bool> m_controls {{C_UP, false}, {C_DOWN, false}, {C_LEFT, false}, {C_RIGHT, false}};
};

class Player
{
public:
    Player(vec2<float> pos, vec2<int> dimensions);
    ~Player();

    void loadAnim(AssetManager* assets);

    void update(float dt, World* world);
    void draw(const vec2<int>& scroll);

    void handleAnimations(float dt, float fallBuf);

    void jump();

    [[nodiscard]] Rectangle getRect() {return Rectangle{m_pos.x, m_pos.y, static_cast<float>(m_dimensions.x), static_cast<float>(m_dimensions.y)};}
    [[nodiscard]] vec2<float> getCenter()
    {
        return vec2<float>{m_pos.x + static_cast<float>(m_dimensions.x) / 2.0f, m_pos.y + static_cast<float>(m_dimensions.y) / 2.0f};
    }

    [[nodiscard]] const vec2<float>& getPos() const {return m_pos;}
    [[nodiscard]] const vec2<int>& getDimensions() const {return m_dimensions;}

    [[nodiscard]] Controller* getController() {return &m_controller;}

    [[nodiscard]] float getJumping() const {return m_jumping;}
    [[nodiscard]] float getFalling() const {return m_falling;}

    void setVelX(const float val) {m_vel.x = val;}
    void setVelY(const float val) {m_vel.y = val;}
    [[nodiscard]] const vec2<float>& getVel() const {return m_vel;}

    [[nodiscard]] bool getFlipped() const {return m_flipped;}

    void setHealth(const float val) {m_health = val;}
    [[nodiscard]] float getHealth() const {return m_health;}
    void setMaxHealth(const float val) {m_maxHealth = val;}
    [[nodiscard]] float getMaxHealth() const {return m_maxHealth;}

    void damage(float amount, float& screenShake);
    void setRecovery(const float amount) {m_recovery = amount;}
    [[nodiscard]] bool getRecovered() const {return m_recovery > m_recoveryTime;}
    [[nodiscard]] float getRecovery() const {return m_recovery;}
    [[nodiscard]] float getRecoverTime() const {return m_recoveryTime;}

    void setOffsetX(const float val) {m_offset.x = val;}
    void setOffsetY(const float val) {m_offset.y = val;}
    void setOffset(const vec2<float>& val) {m_offset = val;}
    [[nodiscard]] vec2<float> getOffset() {return m_offset;}

    void setPos(const vec2<float> pos) {m_pos = pos;}

private:
    vec2<float> m_pos;
    vec2<int> m_dimensions;

    vec2<float> m_vel{0.0f, 0.0f};

    Controller m_controller{};

    float m_jumping{99.0f};
    float m_falling{99.0f};

    Anim* m_idleAnim{nullptr};
    Anim* m_runAnim{nullptr};
    Anim* m_jumpAnim{nullptr};
    Anim* m_landAnim{nullptr};
    Anim* m_damageAnim{nullptr};
    Anim* m_anim{nullptr}; // anim to play

    bool m_flipped{false};
    bool m_grounded{false};

    float m_health{50.f};
    float m_maxHealth{50.f};

    float m_recovery{99.f};
    const float m_recoveryTime{10.f};

    vec2<float> m_offset{0.f, 0.f};

    // free animations
    void free();
};

#endif
