#pragma once

class Entity;

namespace Collision
{
    void CollideBox(Entity *entity, float centerX, float centerY, float width, float height);
    void PostUpdate();
}