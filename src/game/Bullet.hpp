#include "motor/Entity.hpp"

class Bullet : public Entity
{
public:
    virtual void Update() override;

    float x = 0;
    float y = 0;
};