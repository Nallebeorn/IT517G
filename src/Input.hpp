#pragma once

namespace Input
{
    void Init();
    bool WasInputJustPressed(int input);
    bool IsInputDown(int input);
    bool WasInputJustReleased(int input);
    void PostUpdate();
}
