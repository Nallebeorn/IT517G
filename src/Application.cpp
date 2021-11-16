#include "Application.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "fmt/core.h"
#include "logging.hpp"
#include "File.hpp"

static const float vertices[]
{
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f,  0.5f, 0.0f,
};

static GLFWwindow *window;
static uint32_t spriteShader;
static uint32_t quadVao;

static void GlfwErrorCallback(int code, const char *description)
{
    fmt::print("<<GLFW>> ({}) {}\n", code, description);
}

static void APIENTRY GlErrorCallback(GLenum source, GLenum type, uint32_t id, GLenum severity, GLsizei length, const char *message, const void *userParam)
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
        fmt::print("\n<<GL-{}>> {}: {}: ({}) {}\n", severityText, sourceText, typeText, id, message);
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
        fmt::print("<<Shader compilation error>> {}: {}", filename, infoLog);

        return 0;
    }

    return shader;
}

static uint32_t LinkShaders(uint32_t vertShader, uint32_t fragShader)
{
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
        fmt::print("<<Shader linking error>> {}", infoLog);

        return 0;
    }

    return program;
}

void Application::InitAndCreateWindow(int width, int height, const char *title)
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
    window = glfwCreateWindow(1024, 768, "Fördjupningsarbete inom spelutveckling", nullptr, nullptr);

    if (!window)
    {
        const char *errorMessage;
        glfwGetError(&errorMessage);
        Application::ShowError("OpenGL context creation failed", errorMessage);
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
        Application::ShowError("No data", "\"data\" directory not found in working directory.");
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

    glClearColor(0.5, 0.5, 1.0, 1.0);

    glGenVertexArrays(1, &quadVao);

    uint32_t vbo;
    glGenBuffers(1, &vbo);

    glBindVertexArray(quadVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
}

void Application::RunMainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(spriteShader);
        glBindVertexArray(quadVao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();

        fflush(stdout); // Sometimes needed to make sure prints always reach CLION terminal
    }
}

void Application::CleanUp()
{
    glfwTerminate();
}
