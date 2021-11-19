#pragma once

struct GLFWwindow;

namespace Application
{
    void InitAndCreateWindow(int renderWidth, int renderHeight, const char *title);
    void RunMainLoop();
    void CleanUp();
    void ShowError(const char *title, const char *message);
    void SetFullscreen(bool fullscreen);
    GLFWwindow *GetWindow();
}