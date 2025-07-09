#ifndef ENTITIES_H
#define ENTITIES_H

#include "raylib.h"

#include "vec2.hpp"
#include "tiles.hpp"
#include "assets.hpp"
#include "anim.hpp"

#include <string>

class Entity
{
public:
    Entity(const vec2<float>& m_pos, const vec2<int>& m_dimensions, const std::string& name);

    // initialize animations or something
    virtual void init(AssetManager* assets);
    // handle physics
    virtual void update(float dt, World* world);
    // draw entity
    virtual void render(const vec2<int>& scroll);

    // getters
    [[nodiscard]] vec2<float> getPos() const {return m_pos;}
    [[nodiscard]] vec2<int> getDimensions() const {return m_dimensions;}
    [[nodiscard]] std::string_view getName() const {return m_name;}

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

    [[nodiscard]] vec2<float> getCenter() const
    {
        return vec2<float>{m_pos.x + static_cast<float>(m_dimensions.x) / 2.0f, m_pos.y + static_cast<float>(m_dimensions.y) / 2.0f};
    }

protected:
    vec2<float> m_pos;
    vec2<int> m_dimensions;
    std::string m_name;

    vec2<float> m_vel{};

    float m_falling{0.0f};
};

class Blobbo : public Entity
{
public:
    Blobbo(const vec2<float>& pos);
    ~Blobbo();

    virtual void init(AssetManager* assets);
    virtual void update(float dt, World* world);
    virtual void render(const vec2<int>& scroll);

    void handleAnimations(float dt);

private:
    Anim* m_idleAnim{nullptr};
    Anim* m_runAnim{nullptr};
    Anim* m_damage{nullptr};
    Anim* m_hurt{nullptr};
    Anim* m_attackAnim{nullptr};

    Anim* m_anim{nullptr};
};

#endif