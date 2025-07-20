#ifndef PARTICLES_H
#define PARTICLES_H

#include "raylib.h"

#include "vec2.hpp"
#include "tiles.hpp"
#include "anim.hpp"
#include "assets.hpp"

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

struct Shockwave
{
    vec2<float> center;
    float targetRadius;

    float innerRadius{0.0f};
    float outerRadius{0.0f};
};

class ShockwaveManager
{
public:
    ShockwaveManager() = default;
    ~ShockwaveManager();

    void free();
    void update(float dt, vec2<int> scroll);

    void addShockwave(vec2<float> pos, float targetRadius);

private:
    std::vector<Shockwave*> m_shockwaves{};
};

struct Flame
{
    vec2<float> pos;
    vec2<float> vel;
    Anim* anim;
};

class FlameManager
{
public:
    FlameManager(AssetManager* assets);
    ~FlameManager();

    void free();
    void update(float dt, vec2<int> scroll);

    void explode(vec2<float> pos, float intensity);

private:
    Texture2D* m_flameTex;

    std::vector<Flame*> m_flames{};
};

struct Cinder
{
    vec2<float> pos;
    vec2<float> vel;
    float size;
    Color color;
};

class CinderManager
{
public:
    CinderManager(AssetManager* assets);
    ~CinderManager();

    void free();

    void update(float dt, vec2<int> scroll, World* world);

    void renderCinder(Cinder* cinder, vec2<int> scroll);

    void addParticle(vec2<float> pos, vec2<float> vel, Color color);

private:
    Texture2D* m_blank;
    std::vector<Cinder*> m_particles{};
    const float m_startSize{8.f};
};

#endif