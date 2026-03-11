//
// Created by anselme on 13/03/2026.
//
#include <csignal>
#include <fstream>
#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#define ASSERT(x) if (!(x)) raise(SIGTRAP)
#define GlTry(x) gl_clear_error();\
x;\
ASSERT(gl_check_error(#x ,__FILE__, __LINE__))
void gl_clear_error()
{
    while (glGetError() != GL_NO_ERROR) {}
}

bool gl_check_error(const char *function, const char *file, int line)
{
    while (const auto error = glGetError())
    {
        std::cerr << "OpenGL error: " << error << " in " << function << " at " << file << ":" << line << std::endl;
        return false;
    }

    return true;
}


void ProcessInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

std::string ReadFile(const std::string &FilePath)
{
    const auto FullFilePath = std::string(PROJECT_ROOT) + "/" + FilePath;
    std::fstream file(FullFilePath.c_str());
    if (!file.is_open())
    {
        std::cerr << "Failed to open file " << FilePath << std::endl;
        return "";
    }

    std::string content, line;
    while (std::getline(file, line))
    {
        content += line + "\n";
    }

    return content;
}

int CompileShader(const std::string &fileContent, GLenum ShaderType)
{
    const int shader = glCreateShader(ShaderType);
    const char *source = fileContent.c_str();
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        const auto message = static_cast<char *>(alloca(length * sizeof(char)));
        glGetShaderInfoLog(shader, length, &length, message);
        std::cerr << "Failed to compile shader!" << std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

int CreateShaderProgram(const std::string &VertexShaderPath, const std::string &FragmentShaderPath)
{
    const int program = glCreateProgram();
    const auto vsSource = ReadFile(VertexShaderPath);
    const auto fsSource = ReadFile(FragmentShaderPath);
    const auto vs = CompileShader(vsSource, GL_VERTEX_SHADER);
    const auto fs = CompileShader(fsSource, GL_FRAGMENT_SHADER);

    GlTry(glAttachShader(program, vs));
    GlTry(glAttachShader(program, fs));

    GlTry(glLinkProgram(program));
    GlTry(glValidateProgram(program));

    GlTry(glUseProgram(program));

    GlTry(glDeleteShader(vs));
    GlTry(glDeleteShader(fs));

    return program;
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to init glfw" << std::endl;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Lesson -> Rectangle", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress))
    {
        glfwTerminate();
        std::cerr << "Failed to init glad" << std::endl;
    }

    const auto shaderProgram = CreateShaderProgram("Resources/Shaders/Lessons/rectangle/vertex.glsl",
                                             "Resources/Shaders/Lessons/rectangle/fragment.glsl");

    if(!shaderProgram)
    {
        std::cerr << "Failed to create shader program" << std::endl;
        glfwTerminate();
        return 1;
    }

    constexpr float positions[] = {
        -0.5f, -0.5f,
        -0.5f, 0.5f,
        0.5f, 0.5f,

        -0.5f, -0.5f,
        0.5f, 0.5f,
        0.5f, -0.5f
    };

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);

    unsigned int vbo;
    GlTry(glGenBuffers(1, &vbo));
    GlTry(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GlTry(glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW));
    GlTry(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0));

    GlTry(glClearColor(1.f, 0.f, 0.f, 1.f));
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        ProcessInput(window);
        GlTry(        glBindVertexArray(vao));
        GlTry(glUseProgram(shaderProgram));
        GlTry(glDrawArrays(GL_TRIANGLES, 0, 6));
        GlTry(glfwPollEvents());
        GlTry(glfwSwapBuffers(window));
    }

    while (const auto error = glGetError())
    {
        std::cerr << "OpenGL error: " << error << std::endl;
    }

    glfwTerminate();
    return 0;
}
