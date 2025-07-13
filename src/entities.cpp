#include "entities.hpp"
#include "constants.hpp"
#include "util.hpp"

Entity::Entity(const vec2<float>& pos, const vec2<int>& dimensions, const std::string& name)
: m_pos{pos}, m_dimensions{dimensions}, m_name{name}
{
}

// load assets or smth
void Entity::init(AssetManager* assets)
{
    std::cout << "Initialized `" << m_name << "`!\n";
}

// handle physics
void Entity::update(const float dt, World* world, Player* player)
{
    constexpr float gravity {0.15f};
    constexpr float friction {0.7f};

    m_falling += dt;

    m_vel.x += (m_vel.x * friction - m_vel.x) * dt;
    m_vel.y += gravity * dt;
    m_vel.y = std::min(m_vel.y, 8.0f); // plz don't fall through blocks

    vec2<float> movement{m_vel.x * dt, m_vel.y * dt};

    m_pos.x += movement.x;

    std::array<Rectangle, 9> rects{};
    world->getTilesAroundPos(getCenter(), rects);
    for (const Rectangle& rect : rects)
    {
        if (CheckCollisionRecs(rect, getRect()))
        {
            if (movement.x < 0.0f)
            {
                m_pos.x = rect.x + rect.width;
            } else {
                m_pos.x = rect.x - m_dimensions.x;
            }
            m_vel.x = 0.0f;
        }
    }

    // keep entity in level
    if (m_pos.x < 0.0f)
    {
        m_pos.x = 0.0f;
        m_vel.x = 0.0f;
    } else if (m_pos.x + m_dimensions.x > CST::LEVEL_WIDTH * CST::CHUNK_SIZE * CST::TILE_SIZE)
    {
        m_pos.x = CST::LEVEL_WIDTH * CST::CHUNK_SIZE * CST::TILE_SIZE - m_dimensions.x;
    }

    m_pos.y += movement.y;
    // same for y motion
    for (const Rectangle& rect : rects)
    {
        if (CheckCollisionRecs(rect, getRect()))
        {
            if (movement.y < 0.0f)
            {
                m_pos.y = rect.y + rect.height;
            } else {
                m_pos.y = rect.y - m_dimensions.y;
                m_falling = 0.0f;
            }
            m_vel.y = 0.0f;
        }
    }

    // keep player in level
    if (static_cast<int>(m_pos.y) + m_dimensions.y > CST::LEVEL_HEIGHT * CST::CHUNK_SIZE * CST::TILE_SIZE)
    {
        m_pos.y = CST::LEVEL_HEIGHT * CST::CHUNK_SIZE * CST::TILE_SIZE - m_dimensions.y;
    }
}

void Entity::render(const vec2<int>& scroll)
{
    DrawRectangle(static_cast<int>(getRect().x) - scroll.x, static_cast<int>(getRect().y) - scroll.y, m_dimensions.x, m_dimensions.y, RED);
}

// --------- Entity Manager --------- //
EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
    free();
}

void EntityManager::update(const float dt, World* world, Player* player, const vec2<int>& scroll, Blaster* blaster)
{
    constexpr unsigned int numAttackers {15};
    for (std::size_t i{0}; i < m_entities.size(); ++i)
    {
        m_entities[i]->setWandering(i > numAttackers);
        m_entities[i]->update(dt, world, player);

        if (m_entities[i]->getKill())
        {
            delete m_entities[i];
            m_entities[i] = nullptr;
        } else {
            m_entities[i]->render(scroll);
        }
    }

    // clear deleted entities
    m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), [](Entity* entity)
    {
        return entity == nullptr;
    }), m_entities.end());
}

void EntityManager::addEntity(EnemyType type, const vec2<float>& pos, AssetManager* assets)
{
    switch (type)
    {
        case EnemyType::BLOBBO:
            { // NOTE: scope fixes jump to case label error
                Blobbo* blobbo {new Blobbo{pos}};
                blobbo->init(assets);
                m_entities.push_back(blobbo);
                break;
            }
        default:
            {
                Entity* entity {new Entity{pos, {10, 10}, "dummy"}};
                entity->init(assets);
                m_entities.push_back(entity);
                break;
            }
    }
}

void EntityManager::free()
{
    for (std::size_t i{0}; i < m_entities.size(); ++i)
    {
        delete m_entities[i];
        m_entities[i] = nullptr;
    }
    m_entities.clear();
}

// --------- Blobbo --------- //

Blobbo::Blobbo(const vec2<float>& pos)
 : Entity{pos, {6, 7}, "blobbo"}
{
}

Blobbo::~Blobbo()
{
    delete m_idleAnim;
    delete m_runAnim;
    delete m_attackAnim;
    delete m_hurt;
    delete m_damage;
}

void Blobbo::init(AssetManager* assets)
{
    m_idleAnim = new Anim{8, 8, 5, 0.2, true, assets->getTexture("blobbo/idle")};
    m_runAnim = new Anim{8, 8, 4, 0.2, true, assets->getTexture("blobbo/run")};
    m_attackAnim = new Anim{8, 8, 6, 0.5, true, assets->getTexture("blobbo/attack")};
    m_hurt = new Anim{8, 8, 1, 0.1, true, assets->getTexture("blobbo/hurt")};
    m_damage = new Anim{8, 8, 1, 0.1, true, assets->getTexture("blobbo/damage")};

    m_anim = m_idleAnim;

    // bit of randomness
    m_speed = Util::random() * 0.3f + 0.1f;
}

void Blobbo::update(const float dt, World* world, Player* player)
{
    // handle animations
    handleAnimations(dt);

    // basic movement

    if (!m_wandering)
    {
        if (std::abs(player->getPos().x - m_pos.x) < 192.f)
        {
            if (player->getPos().x > m_pos.x + 5.f)
            {
                m_vel.x += m_speed * dt;
                m_flipped = false;
            } else if (player->getPos().x < m_pos.x - 5.f)
            {
                m_vel.x -= m_speed * dt;
                m_flipped = true;
            }
        }
    } else {
        m_walk += dt;
        if (m_walk > m_walkTarget)
        {
            m_walk = 0.0f;
            m_direction = (Util::random() > 0.5f) ? -1 : 1;
            m_walking = !m_walking;
            m_walkTarget = m_walking ? (Util::random() * 100 + 100.f) : (Util::random() * 60.f + 20.f);
        }

        if (m_walking)
        {
            m_vel.x += m_speed * static_cast<float>(m_direction);
        } else {
            m_vel.x += (m_vel.x * 0.1f - m_vel.x) * dt;
        }

        m_flipped = m_direction < 0;
    }

    if (Util::random() < 0.05 * dt)
    {
        if (m_falling < 3.0f)
        {
            m_vel.y = -2.f;
            m_falling = 4.0f;
        }
    }

    // handle beef with player
    if (player->getVel().y > 0.2f)
    {
        if (CheckCollisionRecs(player->getRect(), getRect()) && m_vel.y < 1.0f)
        {
            player->setVelY(-4.f);
        }
    }

    // update physics
    Entity::update(dt, world, player);
}

void Blobbo::render(const vec2<int>& scroll)
{
    m_anim->render({m_pos.x - 1.0f, m_pos.y - 1.0f}, scroll);
    m_anim->setFlipped(m_flipped);
}

void Blobbo::handleAnimations(const float dt)
{
    if (m_falling > 3.0f)
    {
        m_anim = m_runAnim;
    } else if (std::abs(m_vel.x) > 0.1f)
    {
        m_anim = m_runAnim;
    } else {
        m_anim = m_idleAnim;
    }
    m_anim->tick(dt);
}
