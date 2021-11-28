#include "Rendering.hpp"
#include <vector>
#include "Application.hpp"
#include "Draw.hpp"
#include "File.hpp"
#include "Mem.hpp"
#include "logging.hpp"
#include "motor/libs/glad.h"
#include "motor/libs/stb_image.h"

namespace
{
    // Vertex attribute locations
    constexpr u32 aVertexPos = 0;

    constexpr u32 aObjectPos = 1;

    const f32 quadVertices[] {
        0.5f, 0.5f,   // Top-right
        -0.5f, 0.5f,  // Top-left
        -0.5f, -0.5f, // Bottom-left
        0.5f, -0.5f,  // Bottom-right
    };

    const u32 quadIndices[] {
        0, 1, 2,
        2, 3, 0
    };

    u32 spriteShader;

    u32 quadVao;

    u32 atlasTexture;

    u32 instanceVbo;

    struct InstanceData
    {
        f32 x;
        f32 y;
    };

    std::vector<InstanceData> drawBuffer;

    s32 GetSizeOfInstanceBuffer(u32 numElements)
    {
        return (s32)numElements * (s32)sizeof(InstanceData);
    }

    u32 LoadShader(const char *filename, GLenum shaderType)
    {
        char *text = File::LoadIntoScratch(filename);

        if (!text)
        {
            return 0;
        }

        u32 shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &text, nullptr);
        glCompileShader(shader);

        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            int infoLogSize;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogSize);
            auto infoLog = (char *)Mem::AllocScratch(infoLogSize);
            glGetShaderInfoLog(shader, infoLogSize, nullptr, infoLog);
            printf(AC_RED "<<Shader compilation error>> %s: " AC_RESET "%s", filename, infoLog);

            return 0;
        }

        return shader;
    }

    u32 LinkShaders(u32 vertShader, u32 fragShader)
    {
        if (!vertShader || !fragShader)
        {
            return 0;
        }

        u32 program = glCreateProgram();
        glAttachShader(program, vertShader);
        glAttachShader(program, fragShader);
        glLinkProgram(program);

        int success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            int infoLogSize;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);
            auto infoLog = (char *)Mem::AllocScratch(infoLogSize);
            glGetProgramInfoLog(program, infoLogSize, nullptr, infoLog);
            printf(AC_RED "<<Shader linking error>> " AC_RESET "%s", infoLog);

            return 0;
        }

        return program;
    }

    u32 LoadTexture(const char *filename)
    {
        int width, height, numChannels;
        uint8_t *data = stbi_load(filename, &width, &height, &numChannels, 0);
        if (!data)
        {
            printf("stbi_load failed: %s", stbi_failure_reason());
            return 0;
        }

        u32 texture;
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
}

void Rendering::Init(int width, int height)
{
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
        Application::ShowError("Texture error", "Failed to load texture atlas.");
        exit(1);
    }

    glGenVertexArrays(1, &quadVao);

    u32 quadStaticVbo;
    glGenBuffers(1, &quadStaticVbo);
    glGenBuffers(1, &instanceVbo);

    u32 quadStaticEbo;
    glGenBuffers(1, &quadStaticEbo);

    glBindVertexArray(quadVao);

    glBindBuffer(GL_ARRAY_BUFFER, quadStaticVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(aVertexPos, 2, GL_FLOAT, false, 2 * sizeof(f32), nullptr);
    glEnableVertexAttribArray(aVertexPos);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadStaticEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    drawBuffer.reserve(2048);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferData(GL_ARRAY_BUFFER, GetSizeOfInstanceBuffer(drawBuffer.capacity()), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(aObjectPos, 2, GL_FLOAT, false, 2 * sizeof(f32), nullptr);
    glVertexAttribDivisor(aObjectPos, 1);
    glEnableVertexAttribArray(aObjectPos);

    glClearColor(0.5, 0.5, 1.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(spriteShader);
    glUniform2f(glGetUniformLocation(spriteShader, "uViewportSize"), (f32)width, (f32)height);
    glUniform1i(glGetUniformLocation(spriteShader, "uTexture"), 0);
    glBindVertexArray(quadVao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTexture);
}

void Rendering::PreUpdate()
{
    drawBuffer.clear();
}

void Rendering::PostUpdate()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    s32 currentGlBufferSize;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &currentGlBufferSize);
    if (currentGlBufferSize < GetSizeOfInstanceBuffer(drawBuffer.capacity()))
    {
        glBufferData(GL_ARRAY_BUFFER, GetSizeOfInstanceBuffer(drawBuffer.capacity()), nullptr, GL_DYNAMIC_DRAW);
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, GetSizeOfInstanceBuffer(drawBuffer.size()), drawBuffer.data());
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, (s32)drawBuffer.size());
}

void Draw::Sprite(f32 x, f32 y)
{
    InstanceData data {};
    data.x = x;
    data.y = y;
    drawBuffer.push_back(data);
}