#pragma once
#include "typedefs.hpp"

struct GLFWwindow;

namespace Application
{
    void InitAndCreateWindow(s32 renderWidth, s32 renderHeight, const char *title);
    void RunMainLoop();
    void CleanUp();
    void ShowError(const char *title, const char *message);
    void SetFullscreen(bool fullscreen);
    GLFWwindow *GetWindow();
}