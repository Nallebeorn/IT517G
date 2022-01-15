#include "Random.hpp"

#include <ctime>
#include <random>

void Random::Init()
{
    srand(time(0));
}

float Random::Value()
{
    return (float)rand() / (float)RAND_MAX;
}

float Random::Range(float min, float max)
{
    return Random::Value() * (max - min) + min;
}
