#ifndef BLASTERS_H
#define BLASTERS_H

#include "vec2.hpp"
#include "player.hpp"
#include "assets.hpp"
#include "util.hpp"
#include "sparks.hpp"

#include <string>
#include <string_view>

struct Bullet
{
    vec2<float> pos;
    float speed;
    float angle;
    bool kill{false};
    float timer{0.0f};
};

struct BlasterStats
{
    float speed; // bullet speed
    float rate; // fire rate
    float armLength; // arm length
    float halfLength; // half length of bullet
    float damage; // bullet damage
    float knockBack; // bullet damage
    float recoil; // gun recoil animation
    float bulletRange; // bullet rect size
};

enum class Blasters
{
    DEFAULT,
    FIRE_BLASTER,
    CANNON,
    BIG_MODDA,
    EXTERMINATOR,
    NONE
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
    virtual void renderBullets(const vec2<int>& scroll);

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
        4.f, // speed
        9.f, // rate
        7.f, // armLength
        4.f, // halfLength
        5.f, // damage
        3.f, // knockBack
        1.7f, // recoil
        2.f, // bulletRange
    };

protected:
    Player* m_player;
    std::string m_name;

    SparkManager* m_sparkManager{nullptr};

    vec2<float> m_offset;
    vec2<float> m_pos{};
    float m_angle{0.0f};
    bool m_flipped{false};

    float m_recoil{0.0f};
    float m_rate{0.0f};

    Anim* m_anim{nullptr};
    Anim* m_bulletAnim{nullptr};

    std::vector<Bullet*> m_bullets{};

    float m_timer{0.0f};
};

// upgraded regular blaster
class FireBlaster : public Blaster
{
public:
    FireBlaster(Player* player, const std::string& name, const vec2<float>& offset)
     : Blaster{player, name, offset}
    {
    }

    void init(AssetManager* assets)
    {
        m_anim = new Anim{12, 5, 3, 0.5f, true, assets->getTexture("blasters/fire_blaster")};
        m_anim->setOrigin({6.f, 2.5f});
        m_bulletAnim = new Anim{8, 3, 1, 0.1, true, assets->getTexture("bullets/fire_bullet")};
        m_bulletAnim->setOrigin({4.f, 1.5f});
        m_sparkManager = new SparkManager{assets};
        stats = BlasterStats{
            8.f, // speed
            5.f, // rate
            7.f, // armLength
            4.f, // halfLength
            5.f, // damage
            12.f, // knockBack
            1.7f, // recoil
            3.f, // bulletRange
        };
    }
};

class Cannon : public Blaster
{
public:
    Cannon(Player* player, const std::string& name, const vec2<float>& offset)
     : Blaster{player, name, offset}
    {
    }

    void init(AssetManager* assets)
    {
        m_anim = new Anim{12, 5, 1, 0.5f, true, assets->getTexture("blasters/cannon")};
        m_anim->setOrigin({6.f, 2.5f});
        m_bulletAnim = new Anim{6, 6, 1, 0.1, true, assets->getTexture("bullets/ball")};
        m_bulletAnim->setOrigin({3.f, 3.f});
        m_sparkManager = new SparkManager{assets};
        stats = BlasterStats{
            3.f, // speed
            20.f, // rate
            7.f, // armLength
            3.f, // halfLength
            11.f, // damage
            70.f, // knockBack
            2.f, // recoil
            8.f, // bulletRange
        };
    }
    

    void updateBullet(Bullet* bullet, const float dt, World* world)
    {
        bullet->pos.x += std::cos(bullet->angle) * bullet->speed * dt;
        bullet->pos.y += std::sin(bullet->angle) * bullet->speed * dt;
        Tile* tile {world->getTileAt(bullet->pos.x + std::cos(bullet->angle) * stats.halfLength,
                                    bullet->pos.y + std::sin(bullet->angle) * stats.halfLength)};
        if (tile != nullptr)
        {
            if (Util::elementIn<TileType, std::size(SOLID_TILES)>(tile->type, SOLID_TILES.data()))
            {
                bullet->kill = true;
            }
        }

        bullet->timer += dt;
        if (bullet->timer > 600.f)
        {
            bullet->kill = true;
        }
    }

    void renderBullet(Bullet* bullet, const vec2<int>& scroll)
    {
        // only render bullet if it's on the screen
        m_bulletAnim->setAngle(m_bulletAnim->getAngle() + 3.f);
        if (0.f - stats.halfLength * 2.f < bullet->pos.x - static_cast<float>(scroll.x)
            && bullet->pos.x - static_cast<float>(scroll.x) < static_cast<float>(GetScreenWidth()) / CST::SCR_VRATIO + stats.halfLength * 2.f
            && 0.f - stats.halfLength * 2.f < bullet->pos.y - static_cast<float>(scroll.y)
            && bullet->pos.y - static_cast<float>(scroll.y) < static_cast<float>(GetScreenHeight()) / CST::SCR_VRATIO + stats.halfLength * 2.f)
        {
            // m_bulletAnim->setFlipped(std::cos(bullet->angle) < 0.0f);
            m_bulletAnim->render({bullet->pos.x, bullet->pos.y}, scroll);
        }
    }
};

class Exterminator : public Blaster
{
public:
    Exterminator(Player* player, const std::string& name, const vec2<float>& offset)
     : Blaster{player, name, offset}
    {
    }

    void init(AssetManager* assets)
    {
        m_anim = new Anim{12, 5, 1, 0.5f, true, assets->getTexture("blasters/exterminator")};
        m_anim->setOrigin({6.f, 2.5f});
        m_bulletAnim = new Anim{8, 3, 1, 0.1, true, assets->getTexture("bullets/shell")};
        m_bulletAnim->setOrigin({4.f, 1.5f});
        m_sparkManager = new SparkManager{assets};
        stats = BlasterStats
        {
            10.f, // speed
            1.f, // rate
            7.f, // armLength
            4.f, // halfLength
            40.f, // damage
            20.f, // knockBack
            0.1f, // recoil
            6.f, // bulletRange
        };
    }
};

class BigModda : public Blaster
{
public:
    BigModda(Player* player, const std::string& name, const vec2<float>& offset)
     : Blaster{player, name, offset}
    {
    }

    void init(AssetManager* assets)
    {
        m_anim = new Anim{12, 5, 1, 0.5f, true, assets->getTexture("blasters/big_modda")};
        m_anim->setOrigin({6.f, 2.5f});
        m_bulletAnim = new Anim{12, 5, 1, 0.1, true, assets->getTexture("bullets/bomb")};
        m_bulletAnim->setOrigin({6.f, 2.5f});
        m_sparkManager = new SparkManager{assets};
        stats = BlasterStats{
            13.f, // speed
            10.f, // rate
            7.f, // armLength
            4.f, // halfLength
            30.f, // damage
            100.f, // knockBack
            3.f, // recoil
            10.f, // bulletRange
        };
    }
};


#endif
