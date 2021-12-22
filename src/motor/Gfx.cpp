#include "Gfx.hpp"
#include <math.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "Application.hpp"
#include "File.hpp"
#include "Mem.hpp"
#include "logging.hpp"
#include "motor/libs/glad.h"
#include "motor/libs/stb_image.h"

using Gfx::Sprite;

namespace
{
    // Vertex attribute locations
    constexpr uint32 aVertexPos = 0;
    constexpr uint32 aObjectPos = 1;
    constexpr uint32 aSpriteRect = 2;

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

    struct SpriteAtlas
    {
        std::unordered_map<std::string, Sprite> data;
        int32 width;
        int32 height;
        uint32 texture;
    } spriteAtlas {};

    uint32 instanceVbo;

    struct InstanceData
    {
        float x;
        float y;
        float atlasX;
        float atlasY;
        float width;
        float height;
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

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);

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

    uint32 LoadTexture(const char *filename, int32 *outWidth = nullptr, int32 *outHeight = nullptr)
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

        if (outWidth) *outWidth = width;
        if (outHeight) *outHeight = height;

        return texture;
    }

    void LoadAtlas(const char *jsonFilename, SpriteAtlas *outAtlas)
    {
        outAtlas->texture = 0;
        outAtlas->data.clear();

        rapidjson::Document doc;

        char *jsonText = File::LoadIntoScratch(jsonFilename);
        doc.ParseInsitu(jsonText);
        if (doc.HasParseError())
        {
            LOG("JSON parse error @%zu: %s", doc.GetErrorOffset(), rapidjson::GetParseError_En(doc.GetParseError()));
        }

        const char *textureRelativeFilename = doc["meta"]["image"].GetString();
        const char *textureFilename = File::ReplaceFilename(jsonFilename, textureRelativeFilename);
        outAtlas->texture = LoadTexture(textureFilename, &outAtlas->width, &outAtlas->height);

        Sprite *previousSprite = nullptr;
        Sprite *frame0Sprite = nullptr;
        for (auto &frame : doc["frames"].GetArray())
        {
            Sprite sprite {};

            std::string name(frame["filename"].GetString());
            sprite.name = name.c_str();
            const char *frameNumberSep = std::strchr(name.c_str(), '#');
            assert(frameNumberSep);
            sprite.frameNumber = std::strtoll(frameNumberSep + 1, nullptr, 10);

            auto rect = frame["frame"].GetObject();
            sprite.x = rect["x"].GetInt();
            sprite.y = rect["y"].GetInt();
            sprite.width = rect["w"].GetInt();
            sprite.height = rect["h"].GetInt();

            int32 durationMs = frame["duration"].GetInt();
            sprite.duration = durationMs * 0.001f;

            outAtlas->data[std::string(name)] = sprite;
            Sprite *newSprite = &outAtlas->data.at(name);

            if (previousSprite)
            {
                if (sprite.frameNumber == previousSprite->frameNumber + 1)
                {
                    previousSprite->nextFrame = newSprite;
                }
                else
                {
                    previousSprite->nextFrame = frame0Sprite;
                }
            }

            if (sprite.frameNumber == 0)
            {
                frame0Sprite = newSprite;
            }

            previousSprite = newSprite;
        }

        if (previousSprite && frame0Sprite)
        {
            previousSprite->nextFrame = frame0Sprite;
        }
    }
}

void Gfx::Init(int width, int height)
{
    spriteShader = LinkShaders(
        LoadShader("data/sprite.vert", GL_VERTEX_SHADER),
        LoadShader("data/sprite.frag", GL_FRAGMENT_SHADER));

    if (!spriteShader)
    {
        Application::ShowError("Shader error", "Failed to compile or link shaders.");
        exit(1);
    }

    LoadAtlas("data/atlas.json", &spriteAtlas);

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

    glVertexAttribPointer(aObjectPos, 2, GL_FLOAT, false, sizeof(InstanceData), nullptr);
    glVertexAttribDivisor(aObjectPos, 1);
    glEnableVertexAttribArray(aObjectPos);

    glVertexAttribPointer(aSpriteRect, 4, GL_FLOAT, false, sizeof(InstanceData), (void *)offsetof(InstanceData, atlasX));
    glVertexAttribDivisor(aSpriteRect, 1);
    glEnableVertexAttribArray(aSpriteRect);

    glClearColor(0.1, 0.1, 0.2, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(spriteShader);
    glUniform2f(glGetUniformLocation(spriteShader, "uViewportSize"), (float)width, (float)height);
    glUniform1i(glGetUniformLocation(spriteShader, "uTexture"), 0);
    glUniform2f(glGetUniformLocation(spriteShader, "uAtlasSize"), (float)spriteAtlas.width, (float)spriteAtlas.height);
    glBindVertexArray(quadVao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, spriteAtlas.texture);
}

void Gfx::ReloadAssets()
{
    uint32 newSpriteShader = LinkShaders(
        LoadShader("data/sprite.vert", GL_VERTEX_SHADER),
        LoadShader("data/sprite.frag", GL_FRAGMENT_SHADER));

    if (newSpriteShader)
    {
        glDeleteProgram(spriteShader);
        spriteShader = newSpriteShader;
    }
    else
    {
        LOG("Shader recompilation failed. Staying with old shader.");
    }

    glDeleteTextures(1, &spriteAtlas.texture);
    spriteAtlas.texture = LoadTexture("data/atlas.png");

    LOG("Reloaded assets");
}

void Gfx::PreUpdate()
{
    drawBuffer.clear();
}

void Gfx::Render()
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

void Gfx::DrawSprite(const char *sprite, int32 x, int32 y)
{
    DrawSprite(GetSpriteData(sprite), x, y);
}

void Gfx::DrawSprite(Sprite *sprite, int32 x, int32 y)
{
    InstanceData data {};

    data.x = (float)x;
    data.y = (float)y;

    if (!sprite)
    {
        data.width = 16;
        data.height = 16;
    }
    else
    {
        data.atlasX = sprite->x;
        data.atlasY = sprite->y;
        data.width = sprite->width;
        data.height = sprite->height;
    }

    drawBuffer.push_back(data);
}

Sprite *Gfx::GetSpriteData(const char *sprite)
{
    if (!std::strchr(sprite, '#'))
    {
        char *name = (char *)Mem::AllocScratch(std::strlen(sprite) + 8);
        std::strcpy(name, sprite);
        std::strcat(name, "#000");
        sprite = name;
    }

    if (spriteAtlas.data.count(sprite) > 0)
    {
        return &spriteAtlas.data.at(sprite);
    }
    else
    {
        LOG(AC_RED "No sprite named %s", sprite);
        return nullptr;
    }
}
