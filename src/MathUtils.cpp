#include "MathUtils.h"
#include <cmath>

float VecLength(sf::Vector2f a)
{
    return sqrtf(a.x * a.x + a.y * a.y);
}


sf::Vector2f VecNormalized(sf::Vector2f a)
{
    float length = VecLength(a);

    if (length != 0.0f)
        return sf::Vector2f(a.x / length, a.y / length);

    return a;
}

float Approach(float value, float target, float delta)
{
    if (value < target)
        return std::min(value + delta, target);
    
    return std::max(value - delta, target);
}