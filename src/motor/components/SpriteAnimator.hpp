#pragma once

#include "motor/typedefs.hpp"
#include "motor/Gfx.hpp"

class Entity;

class SpriteAnimator
{
public:
    void Update();
    Gfx::Sprite *GetSprite();
    void Play(const char *sprite);
    void PlayOnceThen(const char *first, const char *then);
    void PlayOnceThenDestroy(const char *first, Entity *entityToDestroy);

private:
    Gfx::Sprite *queuedSprite = nullptr;
    Gfx::Sprite *currentSprite = nullptr;
    Entity *queuedDestroy = nullptr;
    int32 frame = 0;
    float timer = 0;
};