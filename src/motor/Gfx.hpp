#pragma once

#include <motor/typedefs.hpp>

namespace Gfx
{
    struct Sprite
    {
        Sprite *nextFrame;
        const char *name;
        int32 frameNumber;
        float duration;
        int32 x;
        int32 y;
        int32 width;
        int32 height;
    };

    void Init(int32 width, int32 height);
    void ReloadAssets();
    void PreUpdate();
    void Render();
    void DrawSprite(const char *sprite, int32 x, int32 y);
    void DrawSprite(Sprite *sprite, int32 x, int32 y);
    Sprite *GetSpriteData(const char *sprite);
}