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