#ifndef BLASTERS_H
#define BLASTERS_H

#include "vec2.hpp"
#include "player.hpp"
#include "assets.hpp"

#include <string>

class Blaster
{
public:
    Blaster(Player* player, const std::string& name, const vec2<float>& offset);
    ~Blaster();

    virtual void init(AssetManager* assets);
    virtual void update(float dt);
    virtual void free();

    virtual void render(const vec2<int>& scroll);

    [[nodiscard]] Player* getPlayer() const {return m_player;}
    [[nodiscard]] std::string_view getName() const {return m_name;}

    void setFlipped(const bool val) {m_flipped = val;}
    [[nodiscard]] bool getFlipped() const {return m_flipped;}

    void setOffset(const vec2<float> offset)
    {
        m_offset = offset;
    }
    [[nodiscard]] vec2<float> getOffset() const {return m_offset;}
    [[nodiscard]] vec2<float> getPos() const {return m_pos;}

protected:
    Player* m_player;
    std::string m_name;

    vec2<float> m_offset;
    vec2<float> m_pos{};
    float m_angle{0.0f};
    bool m_flipped{false};

    float m_recoil{0.0f};
    float m_rate{0.0f};

    Anim* m_anim{nullptr};
};

#endif