#ifndef SPARKS_H
#define SPARKS_H

#include <raylib.h>

#include "vec2.hpp"
#include "assets.hpp"  // for blank texture
#include "util.hpp"  // for DrawTexturePoly

#include <vector>
#include <array>
#include <cmath>
#include <algorithm>

struct Spark
{
    vec2<float> pos;
    float angle;
    float speed;
};

class SparkManager
{
public:
    explicit SparkManager(AssetManager* assets)
     : m_blank{assets->getTexture("blank")}
    {
    }

    // free sparks
    ~SparkManager()
    {
        free();
    }

    // free memory used by sparks
    void free()
    {
        for (std::size_t i{0}; i < m_sparks.size(); ++i)
        {
            delete m_sparks[i];
            m_sparks[i] = nullptr;
        }
        m_sparks.clear();
    }
    // update and render sparks
    void update(const float dt, const vec2<int>& scroll)
    {
        // update & render sparks
        for (std::size_t i{0}; i < m_sparks.size(); ++i)
        {
            bool kill {updateSpark(m_sparks[i], dt)};
            if (!kill)
            {
                renderSpark(m_sparks[i], scroll);
            } else {
                // free spark
                delete m_sparks[i];
                m_sparks[i] = nullptr;
            }
        }

        // clear finished sparks
        m_sparks.erase(std::remove_if(m_sparks.begin(), m_sparks.end(), [](Spark* spark)
        {
            return spark == nullptr;
        }), m_sparks.end());
    }

    // create new spark
    void addSpark(vec2<float> pos, float angle, float speed)
    {
        m_sparks.emplace_back(new Spark{pos, angle, speed});
    }

    // returns kill
    bool updateSpark(Spark* spark, const float dt)
    {
        constexpr float decay{0.2f};

        spark->pos.x += std::cos(spark->angle) * spark->speed * dt;
        spark->pos.y += std::sin(spark->angle) * spark->speed * dt;

        spark->speed -= decay * dt;
        return spark->speed <= 0.f;
    }

    // render spark polygon
    void renderSpark(Spark* spark, const vec2<int>& scroll)
    {
        constexpr float scale{3.0f}; // scale of spark
        constexpr float width{0.5f}; // width between kite wings
        constexpr float snout{0.75f}; // ratio between snout and tail
        constexpr Color color {WHITE};

        const float size{spark->speed * scale};
        // offset for a diamond/kite shape
        std::array<Vector2, 5> points {
            Vector2{0.0f, 0.0f},
            Vector2{std::cos(spark->angle - width * 0.5f) * size * snout, std::sin(spark->angle - width * 0.5f) * size * snout},
            Vector2{std::cos(spark->angle) * size, std::sin(spark->angle) * size},
            Vector2{std::cos(spark->angle + width * 0.5f) * size * snout, std::sin(spark->angle + width * 0.5f) * size * snout},
            Vector2{0.0f, 0.0f}
        };

        // weird polygon rendering
        rlSetTexture(m_blank->id);
        rlBegin(RL_TRIANGLES);

        rlColor4ub(color.r, color.g, color.b, color.a);

        rlTexCoord2f(0.5f, 0.5f);

        rlVertex2f(spark->pos.x - static_cast<float>(scroll.x), spark->pos.y - static_cast<float>(scroll.y));
        rlVertex2f(spark->pos.x - static_cast<float>(scroll.x) + std::cos(spark->angle - width * 0.5f) * size * snout,
                   spark->pos.y - static_cast<float>(scroll.y) + std::sin(spark->angle - width * 0.5f) * size * snout);
        rlVertex2f(spark->pos.x - static_cast<float>(scroll.x) + std::cos(spark->angle) * size,
                   spark->pos.y - static_cast<float>(scroll.y) + std::sin(spark->angle) * size);
        rlVertex2f(spark->pos.x - static_cast<float>(scroll.x), spark->pos.y - static_cast<float>(scroll.y));
        rlVertex2f(spark->pos.x - static_cast<float>(scroll.x) + std::cos(spark->angle + width * 0.5f) * size * snout,
                   spark->pos.y - static_cast<float>(scroll.y) + std::sin(spark->angle + width * 0.5f) * size * snout);
        rlVertex2f(spark->pos.x - static_cast<float>(scroll.x) + std::cos(spark->angle) * size,
                   spark->pos.y - static_cast<float>(scroll.y) + std::sin(spark->angle) * size);
        rlVertex2f(spark->pos.x - static_cast<float>(scroll.x), spark->pos.y - static_cast<float>(scroll.y));
 
        rlEnd();
        rlSetTexture(0);
    }

    [[nodiscard]] const std::vector<Spark*>& getSparks() const {return m_sparks;}

private:
    // blank particle texture
    Texture2D* m_blank;
    // da sparx
    std::vector<Spark*> m_sparks{};
};

#endif
