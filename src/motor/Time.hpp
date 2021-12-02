#pragma once

// Implementation in Application.cpp

namespace Time
{
    constexpr double Delta() { return 1.0 / 60.0; }
    double AtFrameBegin();
    double CurrentRealtime();
}