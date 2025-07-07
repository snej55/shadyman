#include "player.hpp"

Player::Player(const vec2<float> pos, const vec2<int> dimensions)
 : m_pos{pos}, m_dimensions{dimensions}
{
}

void Player::update(const float dt, World* world)
{
    constexpr float gravity {0.3f};
    constexpr float friction {0.7f};
    constexpr float speed {1.2f};

    if (m_controller.getControl(C_RIGHT))
    {
        m_vel.x += speed * dt;
    }
    if (m_controller.getControl(C_LEFT))
    {
        m_vel.x -= speed * dt;
    }
    m_vel.x += (m_vel.x * friction - m_vel.x) * dt;
    m_vel.y += gravity * dt;

    vec2<float> movement {m_vel.x * dt, m_vel.y * dt};

    m_pos.x += movement.x;

    std::array<Rectangle, 9> rects{};
    world->getTilesAroundPos(getCenter(), rects);

    // check collisions
    for (const Rectangle& rect : rects)
    {
        // check if we collided with tile
        if (CheckCollisionRecs(rect, getRect()))
        {
            // player was moving left?
            if (movement.x < 0.0f)
            {
                m_pos.x = rect.x + rect.width; // back bro off
            } else { // hmm must be moving right
                m_pos.x = rect.x - m_dimensions.x; // just squidge in there...
            }
            m_vel.x = 0.0f; // STOP!
        }
    }

    m_pos.y += movement.y;
    // same for y motion
    for (const Rectangle& rect : rects)
    {
        // check if we collided with tile
        if (CheckCollisionRecs(rect, getRect()))
        {
            // player was moving up?
            if (movement.y < 0.0f)
            {
                m_pos.y = rect.y + rect.height; // back bro off
            } else { // hmm must be pelting from the sky
                m_pos.y = rect.y - m_dimensions.y; // do a nice balancing act
            }
            m_vel.y = 0.0f; // STOP!
        }
    }
}

void Player::draw(const vec2<int>& scroll)
{
    Rectangle rect {getRect()};
    DrawRectangle(rect.x - scroll.x, rect.y - scroll.y, rect.width, rect.height, RED);
}