#include "Enemy.hpp"

#include <cmath>

#include <motor/Application.hpp>
#include <motor/Collision.hpp>
#include <motor/Math.hpp>
#include <motor/Random.hpp>
#include <motor/Time.hpp>

#include "EnemyBullet.hpp"

void Enemy::Create()
{
    tag = EntityTag::Enemy;

    animator.Play("Enemy");

    constexpr float speedMin = 30.0f;
    constexpr float speedMax = 50.0f;
    speed = Random::Range(speedMin, speedMax);
    
    shootTimer = 1.0f;
}

void Enemy::Update()
{
    x -= Time::Delta() * speed;

    if (x < -50)
    {
        Destroy();
    }

    animator.Update();
    Gfx::DrawSprite(animator.GetSprite(), x, y);

    if (!dying && !isPendingDelete)
    {
        Collision::CollideBox(this, x, y, 16, 16);

        shootTimer -= Time::Delta();
        if (shootTimer <= 0)
        {
            constexpr int32 numBullets = 10;
            float baseAngle = Random::Range(0.0f, Math::Tau);
            for (int32 i = 0; i < 10; i++)
            {
                float angle = ((float)i / (float)numBullets) * Math::Tau;
                auto *bullet = Application::CreateEntity<EnemyBullet>();
                bullet->x = x;
                bullet->y = y;
                bullet->angle = std::fmod(baseAngle + angle, Math::Tau);
            }

            shootTimer = Random::Range(2.0, 4.0f);
        }
    }
}

void Enemy::OnCollision(Entity *other)
{
    if (!dying && other->tag == EntityTag::PlayerBullet)
    {
        animator.PlayOnceThenDestroy("EnemyDeath", this);
        other->Destroy();
        dying = true;
    }
}
