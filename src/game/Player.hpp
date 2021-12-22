#pragma once

#include "motor/Entity.hpp"
#include "motor/components/SpriteAnimator.hpp"

class Player : public Entity
{
public:
    Player() : x(32), y(32) {}

    void Create();
    virtual void Update() override;

private:
    float x;
    float y;
    float xMoveAccumulator = 0;
    float yMoveAccumulator = 0;
    float fireCooldown = 0;
    SpriteAnimator animator;
};