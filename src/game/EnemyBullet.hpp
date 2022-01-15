#pragma once

#include <motor/Entity.hpp>
#include <motor/components/SpriteAnimator.hpp>

class EnemyBullet : public Entity
{
public:
    float x = 0;
    float y = 0;
    float angle = 0;

private:
    SpriteAnimator animator {};

public:
    virtual void Create();
    virtual void Update() override;
};