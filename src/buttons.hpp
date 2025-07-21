#ifndef BUTTONS_H
#define BUTTONS_H

#include "raylib.h"

#include "assets.hpp"
#include "vec2.hpp"

class Button
{
public:
    Button(vec2<float> pos, vec2<int> dimensions, Texture2D* tex)
     : m_pos{pos}, m_dimensions{dimensions}, m_tex{tex}
    {
    }

    ~Button() = default;

    void update(const float scale)
    {
        Vector2 mousePos = GetMousePosition();
        mousePos.x /= scale;
        mousePos.y /= scale;
        m_hover = CheckCollisionPointRec(mousePos, getRect());
    }

    void render(vec2<int> scroll = {0, 0})
    {
        DrawTexture(*m_tex, static_cast<int>(m_pos.x) - scroll.x, static_cast<int>(m_pos.y) - scroll.y, WHITE);
        if (m_hover)
        {
            DrawRectangle(static_cast<int>(m_pos.x) - scroll.x, static_cast<int>(m_pos.y) - scroll.y, m_dimensions.x, m_dimensions.y, {255, 255, 255, 100});
        }
    }

    [[nodiscard]] Rectangle getRect() const
    {
        return Rectangle{m_pos.x, m_pos.y, static_cast<float>(m_dimensions.x), static_cast<float>(m_dimensions.y)};
    }

    // getters & setters
    void setPosX(const float val) {m_pos.x = val;}
    void setPosY(const float val) {m_pos.y = val;}
    [[nodiscard]] vec2<float> getPos() const {return m_pos;}

    [[nodiscard]] vec2<int> getDimensions() const {return m_dimensions;}
    [[nodiscard]] Texture2D* getTex() const {return m_tex;}

    [[nodiscard]] bool getHover() const {return m_hover;}

private:
    vec2<float> m_pos;
    vec2<int> m_dimensions;
    Texture2D* m_tex;

    bool m_hover{false};
};


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

class Scale
{
public:
    Scale(vec2<float> pos)
     : m_pos{pos}
    {
    }

    ~Scale() = default;

    void update(const float scale)
    {
        vec2<float> mousePos {GetMousePosition().x, GetMousePosition().y};
        mousePos.x /= scale;
        mousePos.y /= scale;
        m_mousePos = mousePos;
    }

    void render(AssetManager* assets, vec2<int> scroll = {0, 0})
    {
        for (int i{0}; i < 4; ++i)
        {
            DrawTexturePro(*assets->getTexture("scale"), {12.f * i, 0, 12.f, 12.f}, {m_pos.x - static_cast<float>(scroll.x) + i * (12.f + m_spacing), m_pos.y - static_cast<float>(scroll.y), 12.f, 12.f}, {0.f, 0.f}, 0.f, WHITE);
            if (CheckCollisionPointRec({m_mousePos.x, m_mousePos.y}, getRect(i)) || static_cast<int>(m_scale) == i + 1)
            {
                DrawRectangle(m_pos.x - static_cast<float>(scroll.x) + i * (12.f + m_spacing), m_pos.y - static_cast<float>(scroll.y), 12.f, 12.f, {255, 255, 255, 100});
            }
        }
    }

    [[nodiscard]] Rectangle getRect(const int num)
    {
        return Rectangle{m_pos.x + num * (12.f + m_spacing), m_pos.y, 12.f, 12.f};
    };

    void click()
    {
        for (int i{0}; i < 4; ++i)
        {
            if (CheckCollisionPointRec({m_mousePos.x, m_mousePos.y}, getRect(i)))
            {
                m_scale = static_cast<float>(i + 1);
            }
        }
    }

    [[nodiscard]] float getScale() const {return m_scale;}

private:
    vec2<float> m_pos;
    float m_scale{4.f};

    vec2<float> m_mousePos{};

    const float m_spacing{2.f};
};

#endif