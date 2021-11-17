#include "Motor.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <stb_image.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "logging.hpp"
#include "File.hpp"

static const float quadVertices[]
{
     0.5f,  0.5f, // Top-right
    -0.5f,  0.5f, // Top-left
    -0.5f, -0.5f, // Bottom-left
     0.5f, -0.5f, // Bottom-right
};

static const uint32_t quadIndices[]
{
    0, 1, 2,
    2, 3, 0,
};

static GLFWwindow *window;
static uint32_t spriteShader;
static uint32_t quadVao;
static uint32_t atlasTexture;

void PlatformInit();

static void GlfwErrorCallback(int code, const char *description)
{
    printf("<<GLFW>> (%d) %s\n", code, description);
}

static void APIENTRY GlErrorCallback(GLenum source, GLenum type, uint32_t id, GLenum severity, GLsizei /*length*/, const char *message, const void * /*userParam*/)
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

static uint32_t LoadShader(const char *filename, GLenum shaderType)
{
    char *text = File::LoadIntoNewBuffer(filename);

    if (!text)
    {
        return 0;
    }

    uint32_t shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &text, nullptr);
    glCompileShader(shader);
    free(text);

    int success;
    constexpr int infoLogSize = 1024;
    char infoLog[infoLogSize];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, infoLogSize, nullptr, infoLog);
        printf(AC_RED "<<Shader compilation error>> %s: " AC_RESET "%s", filename, infoLog);

        return 0;
    }

    return shader;
}

static uint32_t LinkShaders(uint32_t vertShader, uint32_t fragShader)
{
    if (!vertShader || !fragShader)
    {
        return 0;
    }

    uint32_t program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    int success;
    constexpr int infoLogSize = 1024;
    char infoLog[infoLogSize];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, infoLogSize, nullptr, infoLog);
        printf(AC_RED "<<Shader linking error>> " AC_RESET "%s", infoLog);

        return 0;
    }

    return program;
}

static uint32_t LoadTexture(const char *filename)
{
    int width, height, numChannels;
    uint8_t *data = stbi_load(filename, &width, &height, &numChannels, 0);
    if (!data)
    {
        printf("stbi_load failed: %s", stbi_failure_reason());
        return 0;
    }

    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    return texture;

}

void Motor::InitAndCreateWindow(int width, int height, const char *title)
{
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
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!window)
    {
        const char *errorMessage;
        glfwGetError(&errorMessage);
        Motor::ShowError("OpenGL context creation failed", errorMessage);
        exit(1);
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GlErrorCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);

    if (!File::DoesDirectoryExist("data"))
    {
        LOG("Data directory not found!");
        Motor::ShowError("No data", "\"data\" directory not found in working directory.");
        exit(1);
    }

    spriteShader = LinkShaders(
        LoadShader("data/sprite.vert", GL_VERTEX_SHADER),
        LoadShader("data/sprite.frag", GL_FRAGMENT_SHADER));

    if (!spriteShader)
    {
        ShowError("Shader error", "Failed to compile or link shaders.");
        exit(1);
    }

    atlasTexture = LoadTexture("data/nalle.png");
    if (!atlasTexture)
    {
        ShowError("Texture error", "Failed to load texture.");
        exit(1);
    }


    glGenVertexArrays(1, &quadVao);

    uint32_t vbo;
    glGenBuffers(1, &vbo);

    uint32_t ebo;
    glGenBuffers(1, &ebo);

    glBindVertexArray(quadVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glClearColor(0.5, 0.5, 1.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(spriteShader);
    glUniform1i(glGetUniformLocation(spriteShader, "uTexture"), 0);
    glBindVertexArray(quadVao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTexture);
}

void Motor::RunMainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();

        fflush(stdout); // Sometimes needed to make sure messages are printed to terminal without waiting for a buffer to be filled
    }
}

void Motor::CleanUp()
{
    glfwTerminate();
}