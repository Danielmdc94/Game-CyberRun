#pragma once

#include <math.h>
#include <SFML/System.hpp>

float VecLength(sf::Vector2f a);
sf::Vector2f VecNormalized(sf::Vector2f a);
float Approach(float value, float target, float delta);