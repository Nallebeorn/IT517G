#include "Collision.hpp"

#include "Entity.hpp"
#include <vector>


namespace
{
    struct Collider
    {
        Entity *entity;
        float left;
        float top;
        float right;
        float bottom;
    };

    std::vector<Collider> colliders;
}

void Collision::CollideBox(Entity *entity, float centerX, float centerY, float width, float height)
{
    colliders.push_back({ entity, centerX - width * .5f, centerY - height * .5f, centerX + width * .5f, centerY + height * .5f });
}

void Collision::PostUpdate()
{
    for (int32 i = 0; i < (int32)colliders.size() - 1; i++)
    {
        Collider *a = &colliders[i];
        for (int32 j = i + 1; j < colliders.size(); j++)
        {
            Collider *b = &colliders[j];

            if (a->left < b->right && a->right > b->left && a->top < b->bottom && a->bottom > b->top)
            {
                a->entity->OnCollision(b->entity);
                b->entity->OnCollision(a->entity);
            }
        }
    }

    colliders.clear();
}
