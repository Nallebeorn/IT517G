#pragma once

#include "typedefs.hpp"
#include "logging.hpp"

enum class EntityTag
{
    None,
    Player,
    PlayerBullet,
    Enemy,
    EnemyBullet,
};

class Entity
{
public:
    Entity *nextEntity = nullptr;
    Entity *prevEntity = nullptr;
    EntityTag tag = EntityTag::None;
    bool enabled = true;
    bool isPendingDelete = false;

public:
    inline void Create() {}
    inline void Destroy() { isPendingDelete = true; }

    virtual void Update() = 0;
    virtual void OnCollision(Entity *other) {};
};