#include "entities.hpp"
#include "constants.hpp"

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
void Entity::update(const float dt, World* world)
{
    constexpr float gravity {0.15f};
    constexpr float friction {0.7f};

    m_falling += dt;

    m_vel.x += (m_vel.x * friction - m_vel.x) * dt;
    m_vel.y += gravity * dt;
    m_vel.y = std::min(m_vel.y, 8.0f); // plz don't fall through blocks

    vec2<float> movement{m_vel.x * dt, m_vel.y * dt};

    m_vel.x += movement.x;

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
    world->getTilesAroundPos(getCenter(), rects);
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
            }
            m_vel.y = 0.0f;
            m_falling = 0.0f;
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
    DrawRectangle(static_cast<int>(getRect().x), static_cast<int>(getRect().y), m_dimensions.x, m_dimensions.y, RED);
}