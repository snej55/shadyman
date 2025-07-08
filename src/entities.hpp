#ifndef ENTITIES_H
#define ENTITIES_H

#include "raylib.h"

#include "vec2.hpp"
#include "tiles.hpp"

class Entity
{
public:
    Entity(const vec2<float>& m_pos, const vec2<int>& m_dimensions);

    // handle physics
    virtual void update(float dt, World* world);
    // draw entity
    virtual void render(const vec2<int>& scroll);

    // getters
    [[nodiscard]] vec2<float> getPos() const {return m_pos;}
    [[nodiscard]] vec2<int> getDimensions() const {return m_dimensions;}

    [[nodiscard]] vec2<float> getVel() const {return m_vel;}
    [[nodiscard]] float getFalling() const {return m_falling;}

    [[nodiscard]] virtual Rectangle getRect() const
    {
        return Rectangle {
            m_pos.x, m_pos.y,
            static_cast<float>(m_dimensions.x),
            static_cast<float>(m_dimensions.y)
        };
    }

protected:
    vec2<float> m_pos;
    vec2<int> m_dimensions;

    vec2<float> m_vel{};

    float m_falling{0.0f};
};

#endif