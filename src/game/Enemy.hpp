#pragma once

#include <motor/Entity.hpp>
#include <motor/components/SpriteAnimator.hpp>

class Enemy : public Entity
{
public:
    float x = 0;
    float y = 0;

private:
    SpriteAnimator animator {};
    bool dying = false;
    float speed = 0;
    float shootTimer;

public:
    void Create();
    virtual void Update() override;
    virtual void OnCollision(Entity *other) override;
};