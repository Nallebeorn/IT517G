#include "Rendering.hpp"
#include <stdint.h>
#include <cstdlib>
#include "stb_image.h"
#include "glad/glad.h"
#include "File.hpp"
#include "logging.hpp"
#include "Application.hpp"

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

static uint32_t spriteShader;
static uint32_t quadVao;
static uint32_t atlasTexture;
static uint32_t targetFramebuffer;

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

void Rendering::Init(uint32_t framebuffer, int width, int height)
{
    targetFramebuffer = framebuffer;

    spriteShader = LinkShaders(
        LoadShader("data/sprite.vert", GL_VERTEX_SHADER),
        LoadShader("data/sprite.frag", GL_FRAGMENT_SHADER));

    if (!spriteShader)
    {
        Application::ShowError("Shader error", "Failed to compile or link shaders.");
        exit(1);
    }

    atlasTexture = LoadTexture("data/nalle.png");
    if (!atlasTexture)
    {
        Application::ShowError("Texture error", "Failed to load texture.");
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
    glUniform2f(glGetUniformLocation(spriteShader, "uViewportSize"), (float)width, (float)height);
    glBindVertexArray(quadVao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTexture);
}

void Rendering::PreUpdate()
{

}

void Rendering::PostUpdate()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}