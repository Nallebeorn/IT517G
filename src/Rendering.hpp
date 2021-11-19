#pragma once

#include <stdint.h>

namespace Rendering
{
    void Init(uint32_t framebuffer, int width, int height);
    void PreUpdate();
    void PostUpdate();
}