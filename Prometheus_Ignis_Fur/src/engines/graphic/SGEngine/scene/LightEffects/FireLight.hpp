#pragma once

#include "LightEffect.hpp"

#include <math.h>

#include <iostream>

struct FireLight : LightEffect
{
    explicit FireLight(float maxLinear, float minLinear, float speed)
        : m_maxLinear{maxLinear}, m_minLinear{minLinear}, m_speed{speed} {}

    void update(LightParams &params, float const &deltaTime) override final
    {
        float displacement = std::sin(m_speed * deltaTime);

        params.linear += displacement;

        if (params.linear > m_maxLinear)
        {
            params.linear = m_maxLinear;
            m_speed = -m_speed;
        }
        else if (params.linear < m_minLinear)
        {
            params.linear = m_minLinear;
            m_speed = -m_speed;
        }

    }

private:
    float m_maxLinear, m_minLinear, m_speed;
};