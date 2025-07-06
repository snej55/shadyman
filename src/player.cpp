#include "player.hpp"

Player::Player(const vec2<float> pos, const vec2<int> dimensions)
 : m_pos{pos}, m_dimensions{dimensions}
{
}

void Player::update(const float dt)
{
    constexpr float gravity {0.3f};
    constexpr float friction {0.48f};
    constexpr float speed {1.0f};

    if (m_controller.getControl(C_RIGHT))
    {
        m_vel.x += speed * dt;
    }
    if (m_controller.getControl(C_LEFT))
    {
        m_vel.x -= speed * dt;
    }
    m_vel.x += (m_vel.x * friction - m_vel.x) * dt;
}

void Player::draw(const vec2<int>& scroll)
{
    Rectangle rect {getRect()};
    DrawRectangle(rect.x - scroll.x, rect.y - scroll.y, rect.width, rect.height, RED);
}