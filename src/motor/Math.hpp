#pragma once

#include "typedefs.hpp"

namespace Math
{
    constexpr float Pi = 3.14159265358979323846;
    constexpr float Tau = 6.2831853071795862;

    void PixelSmoothMovement(float x, float y, float speedX, float speedY, int32 *outX, int32 *outY);

    template<typename T>
    int32 Sign(T x)
    {
        return (T(0) < x) - (x < T(0));
    }
}