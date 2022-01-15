#include "EnemyBullet.hpp"

#include <cmath>

#include <motor/Collision.hpp>
#include <motor/Math.hpp>
#include <motor/Time.hpp>

#include "ShmupGame.hpp"

void EnemyBullet::Create()
{
    tag = EntityTag::EnemyBullet;
    animator.Play("EnemyBullet");
}

void EnemyBullet::Update()
{
    constexpr float speed = 40.0f;
    float dx = std::cos(angle) * speed * Time::Delta();
    float dy = std::sin(angle) * speed * Time::Delta();

    x += dx;
    y += dy;

    int32 displayX, displayY;
    Math::PixelSmoothMovement(x, y, dx, dy, &displayX, &displayY);

    animator.Update();
    Gfx::DrawSprite("EnemyBullet#001", displayX, displayY);

    Collision::CollideBox(this, displayX, displayY, 2, 2);

    if (x < -10 || x > ShmupGame::width + 10 || y < -10 || y > ShmupGame::height + 10)
    {
        Destroy();
    }
}
