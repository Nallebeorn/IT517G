#include "Math.hpp"

#include <cmath>

void Math::PixelSmoothMovement(float x, float y, float speedX, float speedY, int32 *outX, int32 *outY)
{
    // Pixel-perfect movement smoothing, based on [https://gamedev.stackexchange.com/questions/18787/how-does-one-avoid-the-staircase-effect-in-pixel-art-motion]
    // Prevents jittery staircase effect when moving in non-orthogonal directions
    if (std::abs(speedX) > std::abs(speedY))
    {
        *outX = std::round(x);
        *outY = std::round(y + (*outX - x) * speedY / speedX);
    }
    else
    {
        *outY = std::round(y);
        if (speedY == 0)
        {
            *outX = std::round(x);
        }
        else
        {
            *outX = std::round(x + (*outY - y) * speedX / speedY);
        }
    }
}
