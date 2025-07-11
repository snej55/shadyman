#include "blasters.hpp"

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

void Blaster::update(const float dt)
{
    m_pos = m_player->getCenter();
    m_flipped = m_player->getFlipped();
    m_anim->tick(dt);
    // m_anim->setAngle(m_anim->getAngle() + dt * 15.f);
}

void Blaster::free()
{
    delete m_anim;
    m_anim = nullptr;
}

void Blaster::render(const vec2<int>& scroll)
{
    m_anim->setFlipped(m_flipped);
    m_anim->render({m_pos.x + m_offset.x + (m_flipped ? -m_armLength : m_armLength), m_pos.y + m_offset.y}, scroll);
}