#pragma once

namespace Motor
{
    void InitAndCreateWindow(int width, int height, const char *title);
    void RunMainLoop();
    void CleanUp();
    void ShowError(const char *title, const char *message);
}