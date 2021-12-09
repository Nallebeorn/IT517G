#include "Player.hpp"

#include <cmath>

#include <motor/Math.hpp>
#include "motor/Draw.hpp"
#include "motor/Input.hpp"
#include "motor/Time.hpp"
#include <motor/logging.hpp>

void Player::Update()
{
    float speed = 64;

    if (Input::WasInputJustPressed(GLFW_KEY_SPACE))
    {
        LOG("Pressed space!");
    }
    
    if (Input::WasInputJustReleased(GLFW_KEY_SPACE))
    {
        LOG("Released space!");
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

    Draw::Sprite("Nalle.Fire#000", displayX, displayY);
}

void Player::MoveX(float amount)
{
    xMoveAccumulator += amount;
    int32 move = std::trunc(xMoveAccumulator);

    if (move != 0)
    {
        xMoveAccumulator -= move;
        int32 sign = Math::Sign(move);

        while (move != 0)
        {
            x += sign;
            move -= sign;
        }
    }
}

void Player::MoveY(float amount)
{
    yMoveAccumulator += amount;
    int32 move = std::trunc(yMoveAccumulator);

    if (move != 0)
    {
        yMoveAccumulator -= move;
        int32 sign = Math::Sign(move);

        while (move != 0)
        {
            y += sign;
            move -= sign;
        }
    }
}
