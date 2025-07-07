#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

#include "vec2.hpp"
#include "tiles.hpp"

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
    bool getControl(const Control control) {return m_controls[control];}

private:
    std::map<Control, bool> m_controls {{C_UP, false}, {C_DOWN, false}, {C_LEFT, false}, {C_RIGHT, false}};
};

class Player
{
public:
    Player(vec2<float> pos, vec2<int> dimensions);
    ~Player() = default;

    void update(float dt, World* world);
    void draw(const vec2<int>& scroll);

    Rectangle getRect() {return Rectangle{m_pos.x, m_pos.y, static_cast<float>(m_dimensions.x), static_cast<float>(m_dimensions.y)};}
    vec2<float> getCenter()
    {
        return vec2<float>{m_pos.x + static_cast<float>(m_dimensions.x) / 2.0f, m_pos.y + static_cast<float>(m_dimensions.y) / 2.0f};
    }

    const vec2<float>& getPos() const {return m_pos;}
    const vec2<int>& getDimensions() const {return m_dimensions;}

    Controller* getController() {return &m_controller;}

private:
    vec2<float> m_pos;
    vec2<int> m_dimensions;

    vec2<float> m_vel{0.0f, 0.0f};

    Controller m_controller{};
};

#endif