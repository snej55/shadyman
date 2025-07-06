#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

#include "vec2.hpp"

#include <map>

enum class Control
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Controller
{
public:
    Controller() = default;

    void setControl(const Control control, const bool val) {m_controls[control] = val;}
    bool getControl(const Control control) {return m_controls[control];}

private:
    std::map<Control, bool> m_controls {{Control::UP, false}, {Control::DOWN, false}, {Control::LEFT, false}, {Control::RIGHT, false}};
};

class Player
{
public:
    Player(vec2<float> pos, vec2<int> dimensions);
    ~Player() = default;

    //void update();
    //void draw(vec2<int> scroll);

    Rectangle getRect() {return Rectangle{m_pos.x, m_pos.y, static_cast<float>(m_dimensions.x), static_cast<float>(m_dimensions.y)};}

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