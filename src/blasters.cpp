#include "blasters.hpp"
#include "util.hpp"

Blaster::Blaster(Player* player, const std::string& name, const vec2<float>& offset, const float armLength)
 : m_player{player}, m_name{name}, m_offset{offset}, m_armLength{armLength}
{
}

Blaster::~Blaster()
{
    free();
}

void Blaster::init(AssetManager* assets)
{
    m_anim = new Anim{12, 5, 3, 0.5f, true, assets->getTexture("blasters/default")};
    m_anim->setOrigin({6.f, 2.5f});
}

void Blaster::update(const float dt, World* world)
{
    m_pos = m_player->getCenter();
    m_flipped = m_player->getFlipped();
    m_anim->tick(dt);
    m_anim->setAngle(m_angle);

    // update bullets
    for (std::size_t i{0}; i < m_bullets.size(); ++i)
    {
        updateBullet(m_bullets[i], dt, world);
        if (m_bullets[i]->kill)
        {
            delete m_bullets[i];
            m_bullets[i] = nullptr;
        }
    }

    // clear used bullets
    m_bullets.erase(std::remove_if(m_bullets.begin(), m_bullets.end(), [](Bullet* bullet) {
        return bullet == nullptr;
    }), m_bullets.end());
}

void Blaster::free()
{
    for (std::size_t i{0}; i < m_bullets.size(); ++i)
    {
        delete m_bullets[i];
    }
    m_bullets.clear();
    delete m_anim;
    m_anim = nullptr;
}

void Blaster::render(const vec2<int>& scroll)
{
    // render bullets
    for (std::size_t i{0}; i < m_bullets.size(); ++i)
    {
        renderBullet(m_bullets[i], scroll);
    }

    m_anim->setFlipped(m_flipped);
    m_anim->render({m_pos.x + m_offset.x + (m_flipped ? -m_armLength : m_armLength), m_pos.y + m_offset.y}, scroll);
}

void Blaster::fire()
{
    m_bullets.emplace_back(new Bullet{{
        m_pos.x + m_offset.x + (m_flipped ? -m_armLength : m_armLength) * 2.f, // pos
        m_pos.y + m_offset.y},
        2.f, // speed
        m_flipped ? PI : 0.f}); // angle
}

void Blaster::updateBullet(Bullet* bullet, const float dt, World* world)
{
    constexpr float halfLength {5.f};
    bullet->pos.x += std::cos(bullet->angle) * bullet->speed * dt;
    bullet->pos.y += std::sin(bullet->angle) * bullet->speed * dt;
    Tile* tile {world->getTileAt(bullet->pos.x + std::cos(bullet->angle) * halfLength, 
                                 bullet->pos.y + std::sin(bullet->angle) * halfLength)};
    if (tile != nullptr)
    {
        if (Util::elementIn<TileType, std::size(SOLID_TILES)>(tile->type, SOLID_TILES.data()))
        {
            bullet->kill = true;
        }
    }
}

void Blaster::renderBullet(Bullet* bullet, const vec2<int>& scroll)
{
    DrawRectangle(static_cast<int>(bullet->pos.x) - scroll.x, static_cast<int>(bullet->pos.y) - scroll.y, 10, 10, RED);
}