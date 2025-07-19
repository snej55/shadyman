#ifndef ENTITIES_H
#define ENTITIES_H

#include "raylib.h"

#include "vec2.hpp"
#include "tiles.hpp"
#include "assets.hpp"
#include "anim.hpp"
#include "player.hpp"
#include "blasters.hpp"
#include "sparks.hpp"

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
    virtual void update(float dt, World* world, Player* player, float& screenShake);
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

    void setAttacking(const bool val) {m_attacking = val;}
    [[nodiscard]] bool getAttacking() const {return m_attacking;}

    void setOffsetX(const float val) {m_offset.x = val;}
    void setOffsetY(const float val) {m_offset.y = val;}
    void setOffset(const vec2<float>& val) {m_offset = val;}
    [[nodiscard]] vec2<float> getOffset() {return m_offset;}

    [[nodiscard]] float getDanger() const {return m_danger;}

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
    bool m_attacking{false};

    vec2<float> m_offset{0.f, 0.f};

    const float m_danger{3.0f}; // amount of damage enemy does
};

class EntityManager
{
public:
    EntityManager();
    ~EntityManager();

    void init(AssetManager* assets);

    void free();
    
    void update(float dt, World* world, Player* player, const vec2<int>& scroll, Blaster* blaster, float& screenShake);

    void addEntity(EnemyType type, const vec2<float>& pos, AssetManager* assets);

    [[nodiscard]] SparkManager* getSparkManager() const {return m_sparkManager;}

private:
    std::vector<Entity*> m_entities{};

    // particle vfx managers
    SparkManager* m_sparkManager{nullptr};

};

class Blobbo : public Entity
{
public:
    Blobbo(const vec2<float>& pos);
    ~Blobbo();

    virtual void init(AssetManager* assets);
    virtual void update(float dt, World* world, Player* player, float& screenShake);
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

    float m_speed = 0.05f;

    float m_walk{110.0f};
    float m_walkTarget{100.f};
    int m_direction{1};
    bool m_walking{true};

    const float m_danger{4.f};
};

#endif
