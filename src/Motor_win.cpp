#include "Motor.hpp"
#include <windows.h>

void PlatformInit()
{
//    SetConsoleMode()
}

void Motor::ShowError(const char *title, const char *message)
{
    MessageBoxA(GetActiveWindow(), message, title, MB_OK | MB_ICONERROR);
}