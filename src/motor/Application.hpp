#pragma once
#include "typedefs.hpp"

struct GLFWwindow;
class Entity;

namespace Application
{
    void InitAndCreateWindow(int32 renderWidth, int32 renderHeight, const char *title);
    void RunMainLoop();
    void CleanUp();
    void ShowError(const char *title, const char *message);
    void SetFullscreen(bool fullscreen);
    GLFWwindow *GetWindow();
    void AddEntity(Entity *entity);

    template<typename T, typename... Args>
    T *CreateEntity(Args... args)
    {
        T *newEntity = new T(args...);
        AddEntity(newEntity);
        return newEntity;
    }


}