#include "Input.hpp"
#include <assert.h>
#include "Application.hpp"
#include "GLFW/glfw3.h"
#include "logging.hpp"

static constexpr uint32_t KEY_PRESSED_MASK = 0x01;
static constexpr uint32_t KEY_RELEASED_MASK = 0x02;
static constexpr uint32_t KEY_HELD_MASK = 0x04;

static uint32_t inputState[GLFW_KEY_LAST + 1];

static void GlfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key < 0 || key > GLFW_KEY_LAST)
    {
        LOG("Key %d is out of bounds", key);
        return;
    }

    switch (action)
    {
        case GLFW_PRESS:
            inputState[key] |= KEY_PRESSED_MASK;
            inputState[key] |= KEY_HELD_MASK;
            break;
        case GLFW_RELEASE:
            inputState[key] |= KEY_RELEASED_MASK;
            inputState[key] &= ~KEY_HELD_MASK;
            break;
        default:
            break;
    }
}

void Input::Init()
{
    GLFWwindow *window = Application::GetWindow();
    glfwSetKeyCallback(window, &GlfwKeyCallback);
}

bool Input::WasInputJustPressed(int input)
{
    assert(input >= 0 && input <= GLFW_KEY_LAST);
    return inputState[input] & KEY_PRESSED_MASK;
}

bool Input::IsInputDown(int input)
{
    assert(input >= 0 && input <= GLFW_KEY_LAST);
    return inputState[input] & KEY_HELD_MASK;
}

bool Input::WasInputJustReleased(int input)
{
    assert(input >= 0 && input <= GLFW_KEY_LAST);
    return inputState[input] & KEY_RELEASED_MASK;
}

void Input::PostUpdate()
{
    for (int i = 0; i <= GLFW_KEY_LAST; i++)
    {
        inputState[i] &= ~(KEY_PRESSED_MASK | KEY_RELEASED_MASK);
    }
}

void Input::ClearInputState(int input)
{
    inputState[input] &= ~(KEY_PRESSED_MASK | KEY_RELEASED_MASK);
}
