#ifndef BLASTERS_H
#define BLASTERS_H

#include "vec2.hpp"
#include "player.hpp"
#include "assets.hpp"

#include <string>
#include <string_view>

struct Bullet
{
    vec2<float> pos;
    float speed;
    float angle;
    bool kill{false};
};

struct BlasterStats
{
    float speed;
    float rate;
    float armLength;
    float halfLength;
};

class Blaster
{
public:
    Blaster(Player* player, const std::string& name, const vec2<float>& offset);
    ~Blaster();

    virtual void init(AssetManager* assets);
    virtual void update(float dt, World* world);
    virtual void free();

    virtual void render(const vec2<int>& scroll);

    virtual void fire();
    virtual void updateBullet(Bullet* bullet, float dt, World* world);
    virtual void renderBullet(Bullet* bullet, const vec2<int>& scroll);

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

    const std::vector<Bullet*>& getBullets() const {return m_bullets;}

    [[nodiscard]] virtual Anim* getAnim() const {return m_anim;}
    [[nodiscard]] virtual Anim* getBulletAnim() const {return m_bulletAnim;}

    BlasterStats stats
    {
        5.f, // speed
        1.f, // rate
        7.f, // armLength
        4.f  // halfLength
    };

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
    Anim* m_bulletAnim{nullptr};

    std::vector<Bullet*> m_bullets{};
};

#endif
