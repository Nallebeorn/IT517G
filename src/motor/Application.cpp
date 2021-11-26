#include "Application.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "motor/libs/glad.h"
#include "GLFW/glfw3.h"
#include "logging.hpp"
#include "File.hpp"
#include "Input.hpp"
#include "Rendering.hpp"
#include "Draw.hpp"
#include "Mem.hpp"

namespace
{
u32 viewportFbo;
s32 viewportWidth;
s32 viewportHeight;

struct WindowState
{
    GLFWwindow *glfwWindow;
    s32 width;
    s32 height;
    s32 blitX;
    s32 blitY;
    s32 blitWidth;
    s32 blitHeight;
    s32 rememberedWindowedWidth;
    s32 rememberedWindowedHeight;
    s32 rememberedWindowedX;
    s32 rememberedWindowedY;
    bool fullscreen;
} appWindow;

}

void PlatformInit();

static void GlfwErrorCallback(s32 code, const char *description)
{
    printf("<<GLFW>> (%d) %s\n", code, description);
}

static void GlfwFramebufferResizeCallback(GLFWwindow *, s32 width, s32 height)
{
    appWindow.width = width;
    appWindow.height = height;
    f32 windowAspect = (f32)appWindow.width / (f32)appWindow.height;
    f32 viewportAspect = (f32)viewportWidth / (f32)viewportHeight;
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
}

static void APIENTRY GlErrorCallback(GLenum source, GLenum type, u32 id, GLenum severity, GLsizei /*length*/, const char *message, const void * /*userParam*/)
{
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
        printf("\n%s<<GL-%s>> %s: %s: (%d) %s\n" AC_RESET, color, severityText, sourceText, typeText, id, message);
    }
}

void Application::InitAndCreateWindow(s32 renderWidth, s32 renderHeight, const char *title)
{
    Mem::Init();

    if (!glfwInit())
    {
        LOG("GLFW initialization failed");
        exit(1);
    }

    glfwSetErrorCallback(&GlfwErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_DOUBLEBUFFER, true);

    viewportWidth = renderWidth;
    viewportHeight = renderHeight;

    constexpr s32 renderScale = 1;
    appWindow.width = renderWidth * renderScale;
    appWindow.height = renderHeight * renderScale;

    appWindow.glfwWindow = glfwCreateWindow(appWindow.width, appWindow.height, title, nullptr, nullptr);

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
    glfwSwapInterval(2);

    if (!File::DoesDirectoryExist("data"))
    {
        LOG("Data directory not found!");
        Application::ShowError("No data", "\"data\" directory not found in working directory.");
        exit(1);
    }

    glGenFramebuffers(1, &viewportFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, viewportFbo);
    u32 rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, renderWidth, renderHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Input::Init();
    Rendering::Init(renderWidth, renderHeight);

    fflush(stdout);
}

static f32 spritex;
static f32 spritey;
static f64 prevFrameTime;

void Application::RunMainLoop()
{
    while (!glfwWindowShouldClose(appWindow.glfwWindow))
    {
        Mem::ResetScratch();

        glfwPollEvents();

        if (Input::WasInputJustPressed(GLFW_KEY_ENTER) && Input::IsInputDown(GLFW_KEY_LEFT_ALT))
        {
            SetFullscreen(!appWindow.fullscreen);
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, viewportFbo);
        Rendering::PreUpdate();

        f64 currentTime = glfwGetTime();
        f32 delta = (f32)(currentTime - prevFrameTime);
        f32 speed = 1;
        if (Input::IsInputDown(GLFW_KEY_LEFT)) spritex -= speed;
        if (Input::IsInputDown(GLFW_KEY_RIGHT)) spritex += speed;
        if (Input::IsInputDown(GLFW_KEY_UP)) spritey -= speed;
        if (Input::IsInputDown(GLFW_KEY_DOWN)) spritey += speed;

        Draw::Sprite(200, 50);
        Draw::Sprite(spritex, spritey);

        Rendering::PostUpdate();
        Input::PostUpdate();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, viewportFbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, viewportWidth, viewportHeight,
                          appWindow.blitX, appWindow.blitY, appWindow.blitX + appWindow.blitWidth, appWindow.blitY + appWindow.blitHeight,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glfwSwapBuffers(appWindow.glfwWindow);

        prevFrameTime = currentTime;

        LOG("Frame time: %.2fms | FPS: %.0f", delta * 1000.0, 1.0 / delta);
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
        appWindow. rememberedWindowedHeight = appWindow.height;
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
                             appWindow.rememberedWindowedWidth, appWindow.rememberedWindowedHeight,0);
    }

    glfwSwapInterval(1);
}

GLFWwindow *Application::GetWindow()
{
    return appWindow.glfwWindow;
}
