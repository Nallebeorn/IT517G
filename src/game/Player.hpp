#pragma once

#include "Bullet.hpp"
#include "motor/Entity.hpp"
#include "motor/components/SpriteAnimator.hpp"

class Player : public Entity
{
public:
    Player() {}
    void Create();
    virtual void Update() override;
    virtual void OnCollision(Entity *other) override;

private:
    float x = 0;
    float y = 0;
    float xMoveAccumulator = 0;
    float yMoveAccumulator = 0;
    float fireCooldown = 0;
    float invincibilityTimer = 0;
    int32 lives = 5;
    SpriteAnimator animator {};
};