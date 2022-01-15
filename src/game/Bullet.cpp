#include "Bullet.hpp"

#include <motor/Collision.hpp>
#include <motor/Gfx.hpp>
#include <motor/Time.hpp>
#include <game/ShmupGame.hpp>

void Bullet::Create()
{
    tag = EntityTag::PlayerBullet;
}

void Bullet::Update()
{
    x += Time::Delta() * 200;
    if (x > ShmupGame::width + 32)
    {
        Destroy();
    }

    Gfx::DrawSprite("Bullet", x, y);
    Collision::CollideBox(this, x, y, 4, 4);
}

