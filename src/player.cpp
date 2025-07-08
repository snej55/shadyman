#include "player.hpp"
#include "constants.hpp"

Player::Player(const vec2<float> pos, const vec2<int> dimensions)
 : m_pos{pos}, m_dimensions{dimensions}
{
}

Player::~Player()
{
    free();
}

void Player::loadAnim(AssetManager* assets)
{
    m_idleAnim = new Anim{9, 14, 6, 0.2f, true, assets->getTexture("player/idle")};
    m_runAnim = new Anim{9, 14, 10, 0.35f, true, assets->getTexture("player/run")};
    m_jumpAnim = new Anim{9, 14, 1, 0.1f, true, assets->getTexture("player/jump")};
    m_landAnim = new Anim{9, 14, 8, 0.5f, false, assets->getTexture("player/land")};
    std::cout << "Loaded animations!\n";

    m_anim = m_idleAnim;
}

void Player::update(const float dt, World* world)
{
    // movement constants
    constexpr float gravity {0.25f};
    constexpr float friction {0.67f};
    constexpr float speed {1.0f};
    constexpr float jumpBuf {10.f}; // 0.25s
    constexpr float fallBuf {5.f};
    constexpr float jumpHeight {3.5f};
    constexpr float maxVelY {8.f};

    m_falling += dt;
    m_jumping += dt;

    // x velocity
    if (m_controller.getControl(C_RIGHT))
    {
        m_vel.x += speed * dt;
        m_flipped = false;
    }
    if (m_controller.getControl(C_LEFT))
    {
        m_vel.x -= speed * dt;
        m_flipped = true;
    }
    m_vel.x += (m_vel.x * friction - m_vel.x) * dt;

    // y velocity
    m_vel.y += gravity * dt;
    m_vel.y = std::min(maxVelY, m_vel.y);
    if (m_jumping < jumpBuf)
    {
        if (m_falling < fallBuf)
        {
            // jump!
            m_vel.y = -jumpHeight;
            // can't jump anymore buddy
            m_falling = fallBuf + 1.0f;
            m_jumping = jumpBuf + 1.0f;
        }
    }

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

    // keep player in level
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
            m_falling = 0.0f; // reset falling
        }
    }

    // keep player in level
    if (static_cast<int>(m_pos.y) + m_dimensions.y > CST::LEVEL_HEIGHT * CST::CHUNK_SIZE * CST::TILE_SIZE)
    {
        m_pos.y = CST::LEVEL_HEIGHT * CST::CHUNK_SIZE * CST::TILE_SIZE - m_dimensions.y;
    }

    // update animation
    handleAnimations(dt, fallBuf);
}

void Player::handleAnimations(const float dt, const float fallBuf)
{
    // update animation
    if (m_falling > fallBuf)
    {
        m_anim = m_jumpAnim;
        m_runAnim->reset();
        m_idleAnim->reset();
        m_landAnim->reset();
        m_grounded = false;
    } else if (m_controller.getControl(C_RIGHT) || m_controller.getControl(C_LEFT))
    {
        m_anim = m_runAnim;
        m_landAnim->reset();
        m_idleAnim->reset();
        m_jumpAnim->reset();
    } else if (!m_grounded)
    {
        m_anim = m_landAnim;
        m_runAnim->reset();
        m_idleAnim->reset();
        m_jumpAnim->reset();
        if (m_landAnim->getFinished())
        {
            m_grounded = true;
        }
    } else 
    {
        m_anim = m_idleAnim;
    }
    m_anim->tick(dt);
    m_anim->setFlipped(m_flipped);
}

void Player::jump()
{
    m_jumping = 0.0f;
}

void Player::draw(const vec2<int>& scroll)
{
    Rectangle rect {getRect()};
    // DrawRectangle(rect.x - scroll.x, rect.y - scroll.y, rect.width, rect.height, RED);
    m_anim->render({m_pos.x - 1.0f, m_pos.y}, scroll);
}

void Player::free()
{
    delete m_idleAnim;
    delete m_runAnim;
    delete m_jumpAnim;
    delete m_landAnim;
    std::cout << "Freed animations!\n";
}