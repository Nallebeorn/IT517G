#include "logging.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

void GlfwErrorCallback(int code, const char *description)
{
    LOG("GLFW error {}: {}", code, description);
}

int main()
{
    LOG("Launching IT517G");

    if (!glfwInit())
    {
        LOG("GLFW initialization failed");
        return 1;
    }

    glfwSetErrorCallback(&GlfwErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow *window = glfwCreateWindow(1024, 768, "Fördjupningsarbete inom spelutveckling", nullptr, nullptr);
    if (!window)
    {
        LOG("Window or OpenGL context creation failed");
        return 1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    while (!glfwWindowShouldClose(window))
    {
//        glfwPollEvents();
        glfwWaitEvents();
    }

    return 0;
}
