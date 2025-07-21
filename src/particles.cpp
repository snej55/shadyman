#include "particles.hpp"
#include "util.hpp"

#include <rlgl.h>

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
        constexpr float decay{0.12f};
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

FlameManager::FlameManager(AssetManager* assets)
 : m_flameTex{assets->getTexture("flame")}
{
}

FlameManager::~FlameManager()
{
    free();
}

void FlameManager::free()
{
    for (std::size_t i{0}; i < std::size(m_flames); ++i)
    {
        delete m_flames[i]->anim;
        delete m_flames[i];
    }

    m_flames.clear();
}

void FlameManager::update(const float dt, const vec2<int> scroll)
{
    for (std::size_t i{0}; i < std::size(m_flames); ++i)
    {
        Flame* f{m_flames[i]};
        f->pos.x += f->vel.x * dt;
        f->pos.y += f->vel.y * dt;

        f->anim->tick(dt);

        if (f->anim->getFinished())
        {
            delete m_flames[i]->anim;
            m_flames[i]->anim = nullptr;
            delete m_flames[i];
            m_flames[i] = nullptr;
        } else {
            f->anim->render(f->pos, scroll);
        }
    }

    m_flames.erase(std::remove_if(m_flames.begin(), m_flames.end(), [](Flame* f){return f == nullptr;}), m_flames.end());
}

void FlameManager::explode(vec2<float> pos, float intensity)
{
    for (std::size_t i{0}; i < static_cast<int>(Util::random() * 10.f * intensity + 10.f * intensity); ++i)
    {
        const float angle{Util::random() * static_cast<float>(M_PI) * 2.f};
        const float dist{Util::random() * 12.f * intensity};
        Anim* anim {new Anim{5, 5, 9, 0.4f, false, m_flameTex}};
        anim->setFrame(Util::random() < 0.5f ? 0.f : 1.f); // randomize it a bit
        m_flames.emplace_back(new Flame{{pos.x + std::cos(angle) * dist, pos.y + std::sin(angle) * dist}, {0.0f, -0.9f}, anim});
    }

    for (std::size_t i{0}; i < static_cast<int>(Util::random() * 5.f * intensity + 5.f * intensity); ++i)
    {
        const float angle{Util::random() * static_cast<float>(M_PI) * 2.f};
        Anim* anim {new Anim{5, 5, 9, 1.f, false, m_flameTex}};
        anim->setFrame(Util::random() < 0.5f ? 0.f : 1.f); // randomize it a bit
        m_flames.emplace_back(new Flame{pos, {std::cos(angle) * 5.f, std::sin(angle) * 5.f}, anim});
    }
}

CinderManager::CinderManager(AssetManager* assets)
 : m_blank{assets->getTexture("blank")}
{
}

CinderManager::~CinderManager()
{
    free();
}

void CinderManager::free()
{
    for (std::size_t i{0}; i < std::size(m_particles); ++i)
    {
        delete m_particles[i];
    }
    m_particles.clear();
}

void CinderManager::update(const float dt, const vec2<int> scroll, World* world)
{
    for (std::size_t i{0}; i < m_particles.size(); ++i)
    {
        Cinder* p {m_particles[i]};

        constexpr float bounce{0.7f};
        constexpr float friction{0.99f};
        constexpr float decay{0.1f};
        constexpr float gravity{0.1f};

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
            renderCinder(p, scroll);
        }
    }

    m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(), [](Cinder* p){return p == nullptr;}), m_particles.end());
}

void CinderManager::renderCinder(Cinder* cinder, const vec2<int> scroll)
{
    constexpr float scale{3.0f}; // scale of cinder
    constexpr float width{0.2f}; // width between kite wings
    constexpr float snout{0.75f}; // ratio between snout and tail

    const float angle {std::atan2(cinder->vel.y, cinder->vel.x)};

    BeginBlendMode(BLEND_ADD_COLORS);

    // weird polygon rendering
    rlSetTexture(m_blank->id);
    rlBegin(RL_TRIANGLES);

    rlColor4ub(cinder->color.r, cinder->color.g, cinder->color.b, static_cast<unsigned char>(static_cast<int>(cinder->size / m_startSize * 250.f)));

    rlTexCoord2f(0.5f, 0.5f);

    rlVertex2f(cinder->pos.x - static_cast<float>(scroll.x), cinder->pos.y - static_cast<float>(scroll.y));
    rlVertex2f(cinder->pos.x - static_cast<float>(scroll.x) - std::cos(angle + M_PI * 0.5f), cinder->pos.y - static_cast<float>(scroll.y) - std::sin(angle + M_PI * 0.5f));
    rlVertex2f(cinder->pos.x - static_cast<float>(scroll.x) - cinder->vel.x * 3.f, cinder->pos.y - static_cast<float>(scroll.y) - cinder->vel.y * 3.f);
    rlVertex2f(cinder->pos.x - static_cast<float>(scroll.x), cinder->pos.y - static_cast<float>(scroll.y));
    rlVertex2f(cinder->pos.x - static_cast<float>(scroll.x) - std::cos(angle + M_PI * -0.5f), cinder->pos.y - static_cast<float>(scroll.y) - std::sin(angle + M_PI * -0.5f));
    rlVertex2f(cinder->pos.x - static_cast<float>(scroll.x) - cinder->vel.x * 3.f, cinder->pos.y - static_cast<float>(scroll.y) - cinder->vel.y * 3.f);

    rlEnd();
    rlSetTexture(0);

    EndBlendMode();
}

void CinderManager::addParticle(vec2<float> pos, vec2<float> vel, const Color color)
{
    m_particles.emplace_back(new Cinder{pos, vel, m_startSize - Util::random(), color});
}