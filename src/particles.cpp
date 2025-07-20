#include "particles.hpp"
#include "util.hpp"

KnockbackManager::~KnockbackManager()
{
    free();
}

void KnockbackManager::free()
{
    for (std::size_t i{0}; i < std::size(m_particles); ++i)
    {
        delete m_particles[i];
    }
    m_particles.clear();
}

void KnockbackManager::update(const float dt, const vec2<int>& scroll, World* world)
{
    for (std::size_t i{0}; i < m_particles.size(); ++i)
    {
        Knockback* p {m_particles[i]};

        constexpr float bounce{0.6f};
        constexpr float friction{0.99f};
        constexpr float decay{0.02f};
        constexpr float gravity{0.16f};

        p->pos.x += p->vel.x * dt;
        Tile* tile {world->getTileAt(p->pos.x, p->pos.y)};
        if (tile != nullptr)
        {
            if (Util::elementIn<TileType, std::size(SOLID_TILES)>(tile->type, SOLID_TILES.data()))
            {
                p->pos.x -= p->vel.x * dt;
                p->vel.x *= -bounce;
                p->vel.y *= friction;
            }
        }

        p->pos.y += p->vel.y * dt;
        p->vel.y += gravity * dt;
        tile = world->getTileAt(p->pos.x, p->pos.y);
        if (tile != nullptr)
        {
            if (Util::elementIn<TileType, std::size(SOLID_TILES)>(tile->type, SOLID_TILES.data()))
            {
                p->pos.y -= p->vel.y * dt;
                p->vel.y *= -bounce;
                p->vel.x *= friction;
            }
        }

        p->size -= decay;
        if (p->size <= 0.0f)
        {
            delete p;
            m_particles[i] = nullptr;
        } else {
            DrawPixel(static_cast<int>(p->pos.x) - scroll.x, static_cast<int>(p->pos.y) - scroll.y, {p->color.r, p->color.g, p->color.b, static_cast<unsigned char>(static_cast<int>(p->size / m_startSize * 255.f))});
        }
    }

    m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(), [](Knockback* p){return p == nullptr;}), m_particles.end());
}

void KnockbackManager::addParticle(vec2<float> pos, vec2<float> vel, Color color)
{
    m_particles.emplace_back(new Knockback{
        pos,
        vel,
        m_startSize,
        color
    });
}

SmokeManager::~SmokeManager()
{
    free();
}

void SmokeManager::free()
{
    for (std::size_t i{0}; i < std::size(m_smoke); ++i)
    {
        delete m_smoke[i];
    }
    m_smoke.clear();
}

void SmokeManager::update(const float dt, const vec2<int> scroll)
{
    for (std::size_t i{0}; i < std::size(m_smoke); ++i)
    {
        constexpr float decay{0.1f};
        constexpr float friction{0.9f};
        Smoke* smoke {m_smoke[i]};

        smoke->pos.x += smoke->vel.x * dt;
        smoke->pos.y += smoke->vel.y * dt;

        smoke->vel.x += (smoke->vel.x * friction - smoke->vel.x) * dt;
        smoke->vel.y += (smoke->vel.y * friction - smoke->vel.y) * dt;

        smoke->angle += std::min((smoke->targetAngle - smoke->angle) * 0.5f, static_cast<float>(M_PI) * 0.05f) * dt;

        smoke->size -= decay * dt;
        if (smoke->size <= 0.0f)
        {
            delete m_smoke[i];
            m_smoke[i] = nullptr;
        } else {
            // render smoke
            const float size{m_startSize - smoke->size};
            DrawRectanglePro({smoke->pos.x - (float)scroll.x, smoke->pos.y - (float)scroll.y, size, size}, {size * 0.5f, size * 0.5f}, smoke->angle * 180.f / static_cast<float>(M_PI), {86, 105, 129, static_cast<unsigned char>(static_cast<int>(smoke->size / m_startSize * 250.f))});
        }
    }

    m_smoke.erase(std::remove_if(m_smoke.begin(), m_smoke.end(), [](Smoke* s){return s == nullptr;}), m_smoke.end());
}

void SmokeManager::addSmoke(const vec2<float> pos, const vec2<float> vel)
{
    const float angle{Util::random() * static_cast<float>(M_PI) * 2.f};
    m_smoke.emplace_back(new Smoke
    {
        pos,
        vel,
        angle + static_cast<float>(M_PI) * 6.f,
        angle,
        m_startSize - Util::random()
    });
}

ShockwaveManager::~ShockwaveManager()
{
    free();
}

void ShockwaveManager::free()
{
    for (std::size_t i{0}; i < std::size(m_shockwaves); ++i)
    {
        delete m_shockwaves[i];
    }
    m_shockwaves.clear();
}

void ShockwaveManager::update(const float dt, const vec2<int> scroll)
{
    for (std::size_t i{0}; i < std::size(m_shockwaves); ++i)
    {
        Shockwave* s{m_shockwaves[i]};

        constexpr float outerSpeed{2.f};
        constexpr float innerSpeed{1.5f};

        s->outerRadius += outerSpeed * dt;
        s->innerRadius += innerSpeed * dt;

        if (s->innerRadius >= s->targetRadius)
        {
            delete s;
            m_shockwaves[i] = nullptr;
        } else {
            s->outerRadius = std::min(s->targetRadius, s->outerRadius);
            s->innerRadius = std::min(s->innerRadius, s->targetRadius);
    
            DrawRing({s->center.x - (float)scroll.x, s->center.y - (float)scroll.y}, s->innerRadius, s->outerRadius, 0.0f, 360.f, 60, {255, 253, 240, 255});
        }
    }

    m_shockwaves.erase(std::remove_if(m_shockwaves.begin(), m_shockwaves.end(), [](Shockwave* s){return s == nullptr;}), m_shockwaves.end());
}

void ShockwaveManager::addShockwave(const vec2<float> center, const float targetRadius)
{
    m_shockwaves.emplace_back(new Shockwave{
        center,
        targetRadius
    });
}