#ifndef ANIM_H
#define ANIM_H

#include "raylib.h"

#include "vec2.hpp"

#include <cmath>

class Anim
{
public:
    Anim(int w, int h, int length, float speed, bool loop, Texture2D* texture)
     : m_width{w}, m_height{h}, m_length{length}, m_speed{speed}, m_loop{loop}, m_tex{texture}
    {
    }

    ~Anim() = default;

    void render(const vec2<float>& pos, const vec2<int>& scroll)
    {
        if (m_tex != nullptr)
        {
            DrawTexturePro(*m_tex, getSourceRect(), {std::floor(pos.x - (float)scroll.x), std::floor(pos.y - (float)scroll.y), (float)m_width, (float)m_height}, {std::floor(m_origin.x), std::floor(m_origin.y)}, m_angle, WHITE);
        }
    }

    void reset()
    {
        m_step = 0;
        m_frame = 0.0f;
        m_finished = false;
    }

    void tick(const float dt)
    {
        m_frame += m_speed * dt;
        if (!m_loop)
        {
            if (m_frame > (float)m_length)
            {
                m_finished = true;
            }
            // cap frame to last anim. frame
            m_step = std::min(static_cast<int>(m_frame), m_length - 1);
        } else {
            m_step = static_cast<int>(m_frame) % m_length;
        }
    }

    void setTex(Texture2D* tex) {m_tex = tex;}
    [[nodiscard]] Texture2D* getTex() const {return m_tex;}

    Rectangle getSourceRect() const
    {
        return Rectangle{std::floor(static_cast<float>(m_step * m_width)), 0.f,
            std::floor(static_cast<float>(m_flipped ? -m_width : m_width)), std::floor(static_cast<float>(m_height))};
    }

    vec2<float> getCenter() const
    {
        return {
            /*static_cast<float>(m_width * m_step) + */static_cast<float>(m_width) / 2.0f,
            static_cast<float>(m_height) / 2.0f
        };
    }

    // getters
    [[nodiscard]] int getWidth() const {return m_width;}
    [[nodiscard]] int getHeight() const {return m_height;}
    [[nodiscard]] int getLength() const {return m_length;}

    void setSpeed(const float val)
    {
        m_speed = std::max(0.0f, val);
    }
    [[nodiscard]] float getSpeed() const {return m_speed;}

    void setFlipped(const bool val)
    {
        m_flipped = val;
    }
    [[nodiscard]] bool getFlipped() const {return m_flipped;}

    [[nodiscard]] int getStep() const {return m_step;}
    [[nodiscard]] float getFrame() const {return m_frame;}
    void setFrame(const float frame)
    {
        m_frame = frame;
        m_step = static_cast<int>(m_frame) % m_length;
    }
    [[nodiscard]] bool getFinished() const {return m_finished;}

    void setAngle(const float val) {m_angle = val;}
    [[nodiscard]] float getAngle() const {return m_angle;}

    void setOrigin(const vec2<float>& val) {m_origin = val;}
    [[nodiscard]] vec2<float> getOrigin() const {return m_origin;}

private:
    int m_width;
    int m_height;
    int m_length;
    float m_speed;
    bool m_loop;
    Texture2D* m_tex;

    bool m_flipped{false};

    float m_frame{0.0f};
    int m_step{0};
    bool m_finished{false};

    float m_angle{0.0f};
    vec2<float> m_origin{0.0f, 0.0f};
};

#endif
