#include "Application.hpp"
#include <windows.h>

void PlatformInit()
{
//    SetConsoleMode()
}

void Application::ShowError(const char *title, const char *message)
{
    MessageBoxA(GetActiveWindow(), message, title, MB_OK | MB_ICONERROR);
}