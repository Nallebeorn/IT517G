#pragma once

#include "typedefs.hpp"

namespace Math
{
    template<typename T>
    int32 Sign(T x)
    {
        return (T(0) < x) - (x < T(0));
    }
}