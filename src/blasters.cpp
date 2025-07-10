#include "blasters.hpp"

Blaster::Blaster(Player* player, const std::string& name, const vec2<float>& offset)
 : m_player{player}, m_name{name}, m_offset{offset}
{
}

Blaster::~Blaster()
{
    free();
}


void Blaster::init(AssetManager* assets)
{
    m_anim = new Anim{12, 5, 3, 0.1f, true, assets->getTexture("blasters/default")};
}

void Blaster::update(const float dt)
{
    m_anim->tick(dt);
}

void Blaster::free()
{
    delete m_anim;
    m_anim = nullptr;
}

void Blaster::render(const vec2<int>& scroll)
{
    m_anim->setFlipped(m_flipped);
    m_anim->render({m_pos.x + m_offset.x, m_pos.y + m_offset.y}, scroll);
}