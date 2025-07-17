#ifndef SPARKS_H
#define SPARKS_H

#include <raylib.h>

#include "vec2.hpp"
#include "assets.hpp"  // for blank texture
#include "util.hpp"  // for DrawTexturePoly

#include <vector>
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
        constexpr float decay{0.1f};

        spark->pos.x += std::cos(spark->angle) * spark->speed * dt;
        spark->pos.y += std::sin(spark->angle) * spark->speed * dt;

        spark->speed -= decay * dt;
        return spark->speed <= 0.f;
    }

    // render spark polygon
    void renderSpark(Spark* spark, const vec2<int>& scroll)
    {
        // Util::DrawTexturePoly(Texture2D texture, Vector2 center, Vector2 *points, Vector2 *texcoords, int pointCount, Color tint)
    }

    [[nodiscard]] const std::vector<Spark*>& getSparks() const {return m_sparks;}

private:
    // blank particle texture
    Texture2D* m_blank;
    // da sparx
    std::vector<Spark*> m_sparks{};
};

#endif
