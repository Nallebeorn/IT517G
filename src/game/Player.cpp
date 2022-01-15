#include "Player.hpp"

#include <cmath>
#include <cstdlib>

#include <motor/Application.hpp>
#include <motor/Collision.hpp>
#include <motor/Gfx.hpp>
#include <motor/Input.hpp>
#include <motor/Math.hpp>
#include <motor/Time.hpp>
#include <motor/logging.hpp>

#include "Bullet.hpp"
#include "ShmupGame.hpp"

void Player::Create()
{
    x = 32;
    y = ShmupGame::height / 2;
    tag = EntityTag::Player;
    animator.Play("NalleIdle");
    lives = 5;
}

void Player::Update()
{
    animator.Update();

    if (lives <= 0)
    {
        Gfx::DrawSprite("NalleDead", x, y);
        return;
    }

    float speed = 64;

    if (fireCooldown > 0)
    {
        fireCooldown -= Time::Delta();
    }

    if (Input::IsInputDown(GLFW_KEY_SPACE) && fireCooldown <= 0)
    {
        animator.PlayOnceThen("NalleFire", "NalleIdle");
        fireCooldown = 0.15f;
        x -= 1;
        Bullet *bullet = Application::CreateEntity<Bullet>();
        bullet->x = x + 8;
        bullet->y = y;
    }

    int32 movex = Input::IsInputDown(GLFW_KEY_RIGHT) - Input::IsInputDown(GLFW_KEY_LEFT);
    int32 movey = Input::IsInputDown(GLFW_KEY_DOWN) - Input::IsInputDown(GLFW_KEY_UP);

    if (movex != 0 && movey != 0)
    {
        speed /= std::sqrt(2.0f);
    }

    x += movex * speed * Time::Delta();
    y += movey * speed * Time::Delta();

    x = std::fmin(std::fmax(0.0f, x), ShmupGame::width);
    y = std::fmin(std::fmax(0.0f, y), ShmupGame::height);

    int32 displayX, displayY;

    Math::PixelSmoothMovement(x, y, movex, movey, &displayX, &displayY);

    //// Pixel-perfect movement smoothing, based on [https://gamedev.stackexchange.com/questions/18787/how-does-one-avoid-the-staircase-effect-in-pixel-art-motion]
    //// Prevents jittery staircase effect when moving in non-orthogonal directions
    //if (std::abs(movex) > std::abs(movey))
    //{
    //    displayX = std::round(x);
    //    displayY = std::round(y + (displayX - x) * movey / movex);
    //}
    //else
    //{
    //    displayY = std::round(y);
    //    if (movey == 0)
    //    {
    //        displayX = std::round(x);
    //    }
    //    else
    //    {
    //        displayX = std::round(x + (displayY - y) * movex / movey);
    //    }
    //}

    if (invincibilityTimer > 0)
    {
        invincibilityTimer -= Time::Delta();
    }

    constexpr float blinkPeriod = 0.1f;
    if (invincibilityTimer <= 0 || std::fmod(invincibilityTimer, blinkPeriod) > blinkPeriod * 0.5f)
    {
        Gfx::DrawSprite(animator.GetSprite(), displayX, displayY);
    }

    for (int32 i = 0; i < lives; i++)
    {
        Gfx::DrawSprite("NalleHead", 16 + i * 12, 10);
    }

    Collision::CollideBox(this, displayX - 3, displayY, 4, 4);
}

void Player::OnCollision(Entity *other)
{
    if (other->tag == EntityTag::Enemy || other->tag == EntityTag::EnemyBullet)
    {
        if (invincibilityTimer <= 0)
        {
            lives--;
            invincibilityTimer = 0.75f;

            if (lives <= 0)
            {
                ShmupGame::Get()->EndGame();
            }
        }
    }
}
