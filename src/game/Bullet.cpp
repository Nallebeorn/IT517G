#include "Bullet.hpp"

#include <motor/Gfx.hpp>
#include <motor/Time.hpp>

void Bullet::Update()
{
    x += Time::Delta() * 200;
    if (x > 400)
    {
        this->enabled = false;
    }

    Gfx::DrawSprite("Bullet", x, y);
}
