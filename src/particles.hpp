#ifndef PARTICLES_H
#define PARTICLES_H

#include "raylib.h"

#include "vec2.hpp"
#include "tiles.hpp"

#include <vector>

struct Knockback
{
    vec2<float> pos;
    vec2<float> vel;
    float size;
    Color color;
};

class KnockbackManager
{
public:
    KnockbackManager() = default;
    ~KnockbackManager();

    void free();

    void update(float dt, const vec2<int>& scroll, World* world);

    void addParticle(vec2<float> pos, vec2<float> vel, Color color);

private:
    std::vector<Knockback*> m_particles{};
    const float m_startSize{8.f};
};

struct Smoke
{
    vec2<float> pos;
    vec2<float> vel;
    float targetAngle;
    float angle;
    float size;
};

class SmokeManager
{
public:
    SmokeManager() = default;
    ~SmokeManager();

    void free();
    void update(float dt, vec2<int> scroll);

    void addSmoke(vec2<float> pos, vec2<float> vel);

private:
    std::vector<Smoke*> m_smoke{};
    const float m_startSize{10.f};
};

#endif