#include "Rendering.hpp"
#include <math.h>
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
    constexpr uint32 aVertexPos = 0;

    constexpr uint32 aObjectPos = 1;

    const float quadVertices[] {
        0.5f, 0.5f,   // Top-right
        -0.5f, 0.5f,  // Top-left
        -0.5f, -0.5f, // Bottom-left
        0.5f, -0.5f,  // Bottom-right
    };

    const uint32 quadIndices[] {
        0, 1, 2,
        2, 3, 0
    };

    uint32 spriteShader;

    uint32 quadVao;

    uint32 atlasTexture;

    uint32 instanceVbo;

    struct InstanceData
    {
        float x;
        float y;
    };

    std::vector<InstanceData> drawBuffer;

    int32 GetSizeOfInstanceBuffer(uint32 numElements)
    {
        return (int32)numElements * (int32)sizeof(InstanceData);
    }

    uint32 LoadShader(const char *filename, GLenum shaderType)
    {
        char *text = File::LoadIntoScratch(filename);

        if (!text)
        {
            return 0;
        }

        uint32 shader = glCreateShader(shaderType);
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

    uint32 LinkShaders(uint32 vertShader, uint32 fragShader)
    {
        if (!vertShader || !fragShader)
        {
            return 0;
        }

        uint32 program = glCreateProgram();
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

    uint32 LoadTexture(const char *filename)
    {
        int width, height, numChannels;
        uint8_t *data = stbi_load(filename, &width, &height, &numChannels, 0);
        if (!data)
        {
            printf("stbi_load failed: %s", stbi_failure_reason());
            return 0;
        }

        uint32 texture;
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

    uint32 quadStaticVbo;
    glGenBuffers(1, &quadStaticVbo);
    glGenBuffers(1, &instanceVbo);

    uint32 quadStaticEbo;
    glGenBuffers(1, &quadStaticEbo);

    glBindVertexArray(quadVao);

    glBindBuffer(GL_ARRAY_BUFFER, quadStaticVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(aVertexPos, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(aVertexPos);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadStaticEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    drawBuffer.reserve(2048);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferData(GL_ARRAY_BUFFER, GetSizeOfInstanceBuffer(drawBuffer.capacity()), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(aObjectPos, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);
    glVertexAttribDivisor(aObjectPos, 1);
    glEnableVertexAttribArray(aObjectPos);

    glClearColor(1.0, 1.0, 0.94, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(spriteShader);
    glUniform2f(glGetUniformLocation(spriteShader, "uViewportSize"), (float)width, (float)height);
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
    int32 currentGlBufferSize;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &currentGlBufferSize);
    if (currentGlBufferSize < GetSizeOfInstanceBuffer(drawBuffer.capacity()))
    {
        glBufferData(GL_ARRAY_BUFFER, GetSizeOfInstanceBuffer(drawBuffer.capacity()), nullptr, GL_DYNAMIC_DRAW);
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, GetSizeOfInstanceBuffer(drawBuffer.size()), drawBuffer.data());
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, (int32)drawBuffer.size());
}

void Draw::Sprite(int32 x, int32 y)
{
    InstanceData data {};
    data.x = (float)x;
    data.y = (float)y;
    drawBuffer.push_back(data);
}