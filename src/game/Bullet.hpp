#pragma once

#include "motor/Entity.hpp"

class Bullet : public Entity
{
public:
    Bullet() {};
    void Create();
    virtual void Update() override;

    float x = 0;
    float y = 0;
};