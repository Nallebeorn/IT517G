#include "Player.hpp"

#include <cmath>

#include <motor/Application.hpp>
#include <motor/Gfx.hpp>
#include <motor/Input.hpp>
#include <motor/Math.hpp>
#include <motor/Time.hpp>
#include <motor/logging.hpp>

#include "Bullet.hpp"

void Player::Create()
{
    animator.Play("NalleIdle");
}

void Player::Update()
{
    animator.Update();

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
        auto bullet = Application::CreateEntity<Bullet>();
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

    int32 displayX, displayY;

    //displayX = x;
    //displayY = y;

    // Pixel-perfect movement smoothing, based on [https://gamedev.stackexchange.com/questions/18787/how-does-one-avoid-the-staircase-effect-in-pixel-art-motion]
    // Prevents jittery staircase effect when moving in non-orthogonal directions
    if (std::abs(movex) > std::abs(movey))
    {
        displayX = std::round(x);
        displayY = std::round(y + (displayX - x) * movey / movex);
    }
    else
    {
        displayY = std::round(y);
        if (movey == 0)
        {
            displayX = std::round(x);
        }
        else
        {
            displayX = std::round(x + (displayY - y) * movex / movey);
        }
    }

    Gfx::DrawSprite(animator.GetSprite(), displayX, displayY);
}
