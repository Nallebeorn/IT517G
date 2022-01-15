#include "Application.hpp"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "Collision.hpp"
#include "Entity.hpp"
#include "File.hpp"
#include "Gfx.hpp"
#include "Input.hpp"
#include "Mem.hpp"
#include "Time.hpp"
#include "logging.hpp"
#include "motor/libs/glad.h"
#include <motor/Random.hpp>

namespace
{
    uint32 viewportFbo;
    int32 viewportWidth;
    int32 viewportHeight;

    struct WindowState
    {
        GLFWwindow *glfwWindow;
        int32 width;
        int32 height;
        int32 blitX;
        int32 blitY;
        int32 blitWidth;
        int32 blitHeight;
        int32 rememberedWindowedWidth;
        int32 rememberedWindowedHeight;
        int32 rememberedWindowedX;
        int32 rememberedWindowedY;
        bool fullscreen;
    } appWindow;

    double deltaTime;
    double currentTime;

    Entity *firstEntity;
    Entity *lastEntity;

    void GlfwErrorCallback(int32 code, const char *description)
    {
        LOG("<<GLFW>> (%d) %s\n", code, description);
    }

    void GlfwFramebufferResizeCallback(GLFWwindow *window, int32 width, int32 height)
    {
        appWindow.width = width;
        appWindow.height = height;
        float windowAspect = (float)appWindow.width / (float)appWindow.height;
        float viewportAspect = (float)viewportWidth / (float)viewportHeight;
        if (windowAspect >= viewportAspect)
        {
            appWindow.blitHeight = appWindow.height;
            appWindow.blitWidth = (appWindow.blitHeight * viewportWidth) / viewportHeight;
        }
        else
        {
            appWindow.blitWidth = appWindow.width;
            appWindow.blitHeight = (appWindow.blitWidth * viewportHeight) / viewportWidth;
        }

        appWindow.blitX = (appWindow.width - appWindow.blitWidth) / 2;
        appWindow.blitY = (appWindow.height - appWindow.blitHeight) / 2;
        GLFWmonitor *monitor = glfwGetWindowMonitor(window);
        LOG("New monitor: %s", monitor ? glfwGetMonitorName(monitor) : "<none>");
    }

    void APIENTRY GlErrorCallback(GLenum source, GLenum type, uint32 id, GLenum severity, GLsizei /*length*/, const char *message, const void * /*userParam*/)
    {
        if (id == 131185)
        {
            return;
        }

        const char *sourceText;
        switch (source)
        {
            case GL_DEBUG_SOURCE_API: sourceText = "OpenGL API"; break;
            case GL_DEBUG_SOURCE_APPLICATION: sourceText = "Custom application"; break;
            case GL_DEBUG_SOURCE_OTHER: sourceText = "Other"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceText = "Shader compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY: sourceText = "Third-party"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceText = "Window-system API"; break;
            default: sourceText = "Unknown source"; break;
        }

        const char *typeText;
        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR: typeText = "Error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeText = "Deprecated"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeText = "Undefined behaviour"; break;
            case GL_DEBUG_TYPE_PORTABILITY: typeText = "Portability"; break;
            case GL_DEBUG_TYPE_PERFORMANCE: typeText = "Performance"; break;
            case GL_DEBUG_TYPE_MARKER: typeText = "Marker"; break;
            case GL_DEBUG_TYPE_PUSH_GROUP: typeText = "Push group"; break;
            case GL_DEBUG_TYPE_POP_GROUP: typeText = "Pop group"; break;
            case GL_DEBUG_TYPE_OTHER: typeText = "Other"; break;
            default: typeText = "Unknown type"; break;
        }

        const char *severityText;
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH: severityText = "HIGH"; break;
            case GL_DEBUG_SEVERITY_MEDIUM: severityText = "MEDIUM"; break;
            case GL_DEBUG_SEVERITY_LOW: severityText = "LOW"; break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: severityText = "NOTE"; break;
            default: severityText = "UNKNOWN SEVERITY"; break;
        }

        if (message)
        {
            const char *color = "";
            if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM)
            {
                color = AC_RED;
            }
            LOG("%s<<GL-%s>> %s: %s: (%d)\n\t\t%s\n" AC_RESET, color, severityText, sourceText, typeText, id, message);
        }
    }

    void UpdateEntities()
    {
        for (Entity *entity = firstEntity; entity != nullptr; entity = entity->nextEntity)
        {
            if (entity->enabled)
            {
                entity->Update();
            }
        }
    }

    void DeletePendingEntities()
    {
        for (Entity *entity = firstEntity; entity != nullptr;)
        {
            Entity *nextEntity = entity->nextEntity;

            if (entity->isPendingDelete)
            {
                if (entity->prevEntity)
                {
                    entity->prevEntity->nextEntity = entity->nextEntity;
                }

                if (entity->nextEntity)
                {
                    entity->nextEntity->prevEntity = entity->prevEntity;
                }

                if (entity == firstEntity)
                {
                    firstEntity = entity->nextEntity;
                }

                if (entity == lastEntity)
                {
                    lastEntity = entity->prevEntity;
                }

                delete entity;
            }

            entity = nextEntity;
        }
    }

    void Update()
    {
        static bool paused;
        constexpr int32 dbgKeyReload = GLFW_KEY_F5;
        constexpr int32 dbgKeyPause = GLFW_KEY_F7;
        constexpr int32 dbgKeyStep = GLFW_KEY_F8;

        Mem::ResetScratch();

        currentTime += Time::Delta();

        glfwPollEvents();

        if (Input::WasInputJustPressed(dbgKeyReload))
        {
            Gfx::ReloadAssets();
        }

        if (Input::WasInputJustPressed(dbgKeyPause))
        {
            paused = !paused;
            LOG("%s", paused ? "DEBUG PAUSED" : "DEBUG UNPAUSED");
        }

        if (paused)
        {
            if (Input::WasInputJustPressed(dbgKeyStep))
            {
                LOG("STEP FRAME");
            }
            else
            {
                Input::ClearInputState(dbgKeyPause);
                return;
            }
        }

        if (Input::WasInputJustPressed(GLFW_KEY_ENTER) && Input::IsInputDown(GLFW_KEY_LEFT_ALT))
        {
            Application::SetFullscreen(!appWindow.fullscreen);
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, viewportFbo);

        Gfx::PreUpdate();
        UpdateEntities();
        Collision::PostUpdate();
        DeletePendingEntities();
        Input::PostUpdate();
    }
}

void PlatformInit();

void Application::InitAndCreateWindow(int32 renderWidth, int32 renderHeight, const char *title)
{
    Mem::Init();

    currentTime = 0;

    if (!glfwInit())
    {
        Application::ShowError("GLFW initialization failed", "GLFW initialization failed");
        exit(1);
    }

    double initStartTime = glfwGetTime();

    glfwSetErrorCallback(&GlfwErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_DOUBLEBUFFER, true);

    viewportWidth = renderWidth;
    viewportHeight = renderHeight;

    constexpr int32 renderScale = 4;
    appWindow.width = renderWidth * renderScale;
    appWindow.height = renderHeight * renderScale;

    appWindow.glfwWindow = glfwCreateWindow(appWindow.width, appWindow.height, title, nullptr, nullptr);

    LOG(AC_GREEN "Create window took %.3fms", (glfwGetTime() - initStartTime) * 1000);
    initStartTime = glfwGetTime();

    if (!appWindow.glfwWindow)
    {
        const char *errorMessage;
        glfwGetError(&errorMessage);
        Application::ShowError("OpenGL context creation failed", errorMessage);
        exit(1);
    }

    glfwMakeContextCurrent(appWindow.glfwWindow);

    PlatformInit();

    glfwSetFramebufferSizeCallback(appWindow.glfwWindow, &GlfwFramebufferResizeCallback);
    GlfwFramebufferResizeCallback(appWindow.glfwWindow, appWindow.width, appWindow.height);

    SetFullscreen(false);

    gladLoadGL(glfwGetProcAddress);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDisable(GL_MULTISAMPLE);
    glDebugMessageCallback(GlErrorCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
    glViewport(0, 0, renderWidth, renderHeight);
    glfwSwapInterval(1);

    if (!File::DoesDirectoryExist("data"))
    {
        LOG("Data directory not found!");
        Application::ShowError("No data", "\"data\" directory not found in working directory.");
        exit(1);
    }

    glGenFramebuffers(1, &viewportFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, viewportFbo);
    uint32 rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, renderWidth, renderHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Input::Init();
    Gfx::Init(renderWidth, renderHeight);
    Random::Init();

    double initDuration = glfwGetTime() - initStartTime;
    LOG(AC_GREEN "Init took %.3fms", initDuration * 1000);

    fflush(stdout);
}

void Application::RunMainLoop()
{
    constexpr double targetFrameRate = 60.0;
    constexpr double minFrameRate = 15.0;

    constexpr double snapTimesteps[] {
        1.0 / 15.0,
        1.0 / 20.0,
        1.0 / 30.0,
        1.0 / 60.0,
        1.0 / 120.0,
    };
    constexpr double snapMaxError = 0.002;

    double timeAccumulator = 0;
    double prevFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(appWindow.glfwWindow))
    {
        double newTime= glfwGetTime();
        deltaTime = newTime - prevFrameTime;
        prevFrameTime = newTime;
        deltaTime = std::clamp(deltaTime, 0.0, 1.0 / minFrameRate);

        for (double snap : snapTimesteps)
        {
            if (std::abs(deltaTime - snap) < snapMaxError)
            {
                deltaTime = snap;
                break;
            }
        }

        timeAccumulator += deltaTime;

        while (timeAccumulator > 1.0 / targetFrameRate)
        {
            timeAccumulator -= 1.0 / targetFrameRate;
            Update();
        }

        //LOG("FPS: %f", 1.0 / deltaTime);

        Gfx::Render();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, viewportFbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, viewportWidth, viewportHeight,
                          appWindow.blitX, appWindow.blitY, appWindow.blitX + appWindow.blitWidth, appWindow.blitY + appWindow.blitHeight,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glfwSwapBuffers(appWindow.glfwWindow);
    }
}

void Application::CleanUp()
{
    glfwTerminate();
    Mem::PrintScratchStats();
}

void Application::SetFullscreen(bool fullscreen)
{
    if (fullscreen && !appWindow.fullscreen)
    {
        appWindow.fullscreen = true;
        appWindow.rememberedWindowedWidth = appWindow.width;
        appWindow.rememberedWindowedHeight = appWindow.height;
        glfwGetWindowPos(appWindow.glfwWindow, &appWindow.rememberedWindowedX, &appWindow.rememberedWindowedY);

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(appWindow.glfwWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else if (!fullscreen && appWindow.fullscreen)
    {
        appWindow.fullscreen = false;
        glfwSetWindowMonitor(appWindow.glfwWindow, nullptr,
                             appWindow.rememberedWindowedX, appWindow.rememberedWindowedY,
                             appWindow.rememberedWindowedWidth, appWindow.rememberedWindowedHeight, 0);
    }

    glfwSwapInterval(1);
}

GLFWwindow *Application::GetWindow()
{
    return appWindow.glfwWindow;
}

void Application::AddEntity(Entity *entity)
{
    entity->nextEntity = nullptr;
    entity->prevEntity = lastEntity;

    if (!firstEntity)
    {
        firstEntity = entity;
    }

    if (lastEntity)
    {
        lastEntity->nextEntity = entity;
    }

    lastEntity = entity;
}

void Application::DestroyAllEntities()
{
    for (Entity *entity = firstEntity; entity != nullptr; entity = entity->nextEntity)
    {
        entity->Destroy();
    }
}

double Time::AtFrameBegin()
{
    return currentTime;
}

double Time::CurrentRealtime()
{
    return glfwGetTime();
}