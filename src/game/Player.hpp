#pragma once

#include "motor/Entity.hpp"

class Player : public Entity
{
public:
    Player() : x(32), y(32) {}

    virtual void Update() override;
    void MoveX(float amount);
    void MoveY(float amount);

private:
    float x;
    float y;
    float xMoveAccumulator = 0;
    float yMoveAccumulator = 0;
};