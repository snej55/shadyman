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

    m_pos.x += m_vel.x * dt;
    std::array<Rectangle, 9> rects{};
    world->getTilesAroundPos(getCenter(), rects);
    for (const Rectangle& rect : rects)
    {
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, WHITE);
    }

    m_vel.y += gravity;
    // m_pos.y += m_vel.y * dt;
}

void Player::draw(const vec2<int>& scroll)
{
    Rectangle rect {getRect()};
    DrawRectangle(rect.x - scroll.x, rect.y - scroll.y, rect.width, rect.height, RED);
}