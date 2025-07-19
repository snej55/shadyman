#ifndef BUTTONS_H
#define BUTTONS_H

#include "raylib.h"

#include "assets.hpp"
#include "vec2.hpp"

enum class TickState
{
    TICK,
    TICK_HOVER,
    TICK_EMPTY,
    TICK_EMPTY_HOVER,
    NONE
};

class Tick
{
public:
    Tick(const vec2<float> pos)
     : m_pos{pos}
    {
    }

    ~Tick() = default;

    void update(const float scale)
    {
        vec2<float> mousePos {GetMousePosition().x, GetMousePosition().y};
        mousePos.x /= scale;
        mousePos.y /= scale;

        m_hover = CheckCollisionPointRec({mousePos.x, mousePos.y}, getRect());
        if (m_hover)
        {
            if (m_selected)
            {
                m_state = TickState::TICK_HOVER;
            } else {
                m_state = TickState::TICK_EMPTY_HOVER;
            }
        } else {
            if (m_selected)
            {
                m_state = TickState::TICK;
            } else {
                m_state = TickState::TICK_EMPTY;
            }
        }
    }

    Texture2D* getTexture(AssetManager* assets)
    {
        switch (m_state)
        {
            case TickState::TICK:
                return assets->getTexture("tick");
            case TickState::TICK_HOVER:
                return assets->getTexture("tick_hover");
            case TickState::TICK_EMPTY:
                return assets->getTexture("tick_empty");
            case TickState::TICK_EMPTY_HOVER:
                return assets->getTexture("tick_empty_hover");
            default:
                return nullptr;
        }
    }

    void render(AssetManager* assets, vec2<int> scroll = {0, 0})
    {
        Texture2D* tex {getTexture(assets)};
        if (tex != nullptr)
        {
            DrawTexture(*tex, static_cast<int>(m_pos.x) - scroll.x, static_cast<int>(m_pos.y) - scroll.y, WHITE);
        }
    }

    [[nodiscard]] Rectangle getRect() const
    {
        return Rectangle{m_pos.x + 2.f, m_pos.y + 2.f, 12.f, 12.f};
    }

    void setSelected(const bool val) {m_selected = val;}
    [[nodiscard]] bool getSelected() const {return m_selected;}

    [[nodiscard]] bool getHover() const {return m_hover;}

private:
    vec2<float> m_pos;

    TickState m_state{TickState::TICK};
    bool m_selected{true};
    bool m_hover{false};
};

#endif