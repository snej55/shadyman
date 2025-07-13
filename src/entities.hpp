#ifndef ENTITIES_H
#define ENTITIES_H

#include "raylib.h"

#include "vec2.hpp"
#include "tiles.hpp"
#include "assets.hpp"
#include "anim.hpp"
#include "player.hpp"
#include "blasters.hpp"

#include <string>

enum class EnemyType
{
    BLOBBO,
    NONE
};

class Entity
{
public:
    Entity(const vec2<float>& m_pos, const vec2<int>& m_dimensions, const std::string& name);

    // initialize animations or something
    virtual void init(AssetManager* assets);
    // handle physics
    virtual void update(float dt, World* world, Player* player);
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

    void damage(float amount);

    [[nodiscard]] float getHealth() const {return m_health;}
    [[nodiscard]] float getMaxHealth() const {return m_maxHealth;}
    [[nodiscard]] bool getKill() const {return m_health < 0.f;}

    void setWandering(const bool val) {m_wandering = val;}
    [[nodiscard]] bool getWandering() const {return m_wandering;}

    [[nodiscard]] float getRecoveryTime() const {return m_recoveryTime;}
    [[nodiscard]] float getRevovery() const {return m_recovery;}
    [[nodiscard]] bool getRecovered() const {
        return m_recoveryTime < m_recovery;
    }

protected:
    vec2<float> m_pos;
    vec2<int> m_dimensions;
    std::string m_name;

    vec2<float> m_vel{};

    float m_falling{0.0f};

    float m_health{10.f};
    const float m_maxHealth{10.f};
    const float m_recoveryTime {10.f};
    float m_recovery{99.f};

    bool m_wandering {false};
};

class EntityManager
{
public:
    EntityManager();
    ~EntityManager();

    void update(float dt, World* world, Player* player, const vec2<int>& scroll, Blaster* blaster);

    void addEntity(EnemyType type, const vec2<float>& pos, AssetManager* assets);

private:
    std::vector<Entity*> m_entities{};

    void free();
};

class Blobbo : public Entity
{
public:
    Blobbo(const vec2<float>& pos);
    ~Blobbo();

    virtual void init(AssetManager* assets);
    virtual void update(float dt, World* world, Player* player);
    virtual void render(const vec2<int>& scroll);

    void handleAnimations(float dt);

    void setFlipped(const bool val) {m_flipped = val;}
    [[nodiscard]] bool getFlipped() const {return m_flipped;}

private:
    Anim* m_idleAnim{nullptr};
    Anim* m_runAnim{nullptr};
    Anim* m_damage{nullptr};
    Anim* m_hurt{nullptr};
    Anim* m_attackAnim{nullptr};

    Anim* m_anim{nullptr};

    bool m_flipped{false};

    float m_speed = 0.2f;

    float m_walk{110.0f};
    float m_walkTarget{100.f};
    int m_direction{1};
    bool m_walking{true};
};

#endif
