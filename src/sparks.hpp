#ifndef SPARKS_H
#define SPARKS_H

#include <raylib.h>

#include "vec2.hpp"

#include <vector>

struct Spark
{
    vec2<float> pos;
    float angle;
    float speed;
};

class SparkManager
{
public:
    SparkManager() = default;
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
        constexpr float scale {1.f};
    }

    // create new spark
    void addSpark(vec2<float> pos, float angle, float speed);

    // returns kill
    bool updateSpark(Spark* spark, const float dt);
    // render spark polygon
    void renderSpark(Spark* spark, const vec2<int>& scroll);

    [[nodiscard]] const std::vector<Spark*>& getSparks() const {return m_sparks;}

private:
    std::vector<Spark*> m_sparks{};
};

#endif
