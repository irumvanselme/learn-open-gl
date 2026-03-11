//
// Created by anselme on 13/03/2026.
//

#include<iostream>
#include <fstream>
#include <string>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <csignal>

#define ASSERT(x) if (!(x)) raise(SIGTRAP)
#define GlTry(x) gl_clear_error();\
x;\
ASSERT(gl_check_error(#x ,__FILE__, __LINE__))
void gl_clear_error()
{
    while (glGetError() != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error: " << glGetError() << std::endl;
    }
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
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, true);
    }
}

std::string ReadFileContent(const std::string &filePath)
{
    const auto FullFilePath = std::string(PROJECT_ROOT) + "/" + filePath;
    std::fstream file(FullFilePath.c_str());
    std::string content;
    while (file.good())
    {
        std::string line;
        std::getline(file, line);
        content += line + "\n";
    }

    return content;
}

int main()
{
    // Init glfw
    if (!glfwInit())
    {
        std::cout << "Failed to init glfw" << std::endl;
        return 1;
    };


    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow *window = glfwCreateWindow(1280, 720, "Hello World", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    // init glad
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cout << "Failed to init glad" << std::endl;
        glfwTerminate();
        return 1;
    }

    // Load the shaders
    auto vertexShader = ReadFileContent("Resources/Shaders/Lessons/triangle/vertex.glsl");
    std::cout << vertexShader << std::endl;
    auto fragmentShader = ReadFileContent("Resources/Shaders/Lessons/triangle/fragment.glsl");
    std::cout << fragmentShader << std::endl;

    int shaderProgram = glCreateProgram();
    // 1. Vertex Shader
    int vs = glCreateShader(GL_VERTEX_SHADER);
    auto vsSourceStr = vertexShader.c_str();
    glShaderSource(vs, 1, &vsSourceStr, nullptr);
    glCompileShader(vs);
    int vsCompiled;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &vsCompiled);
    if (!vsCompiled)
    {
        int length;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &length);
        const auto message = static_cast<char *>(alloca(length * sizeof(char)));
        glGetShaderInfoLog(vs, length, &length, message);
        std::cerr << "Failed to compile vertex shader!" <<
                std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(vs);
        return 1;
    }



    // 2. Fragment shader
    int fs = glCreateShader(GL_FRAGMENT_SHADER);
    auto fsSourceString = fragmentShader.c_str();
    glShaderSource(fs, 1, &fsSourceString, nullptr);
    glCompileShader(fs);
    int fsCompiled;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &fsCompiled);
    if (!fsCompiled)
    {
        int length;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &length);
        const auto message = static_cast<char *>(alloca(length * sizeof(char)));
        glGetShaderInfoLog(fs, length, &length, message);
        std::cerr << "Failed to compile fragment shader!" <<
                std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(fs);
        return 1;
    }

    glUseProgram(shaderProgram);
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);

    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);

    glDeleteShader(fs);
    glDeleteShader(vs);


    constexpr float positions[] = {
        -.5f, -.3f,
        .6f, .4f,
        .9f, -.3f,
    };

    unsigned int vao;
    GlTry(glGenVertexArrays(1, &vao));
    GlTry(glBindVertexArray(vao));
    GlTry(glEnableVertexAttribArray(0));

    // Generate the Vertex Buffer
    unsigned int vbo;
    GlTry(glGenBuffers(1, &vbo));
    GlTry(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GlTry(glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW));


    // We need to tell our shader hot to process our buffer;
    GlTry(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0));

    GlTry(glClearColor(1, 0, 0, 1));

    while (!glfwWindowShouldClose(window))
    {
        // This needs to be inside the loop because each frame needs a new frame buffer.
        // If we put outside
        glClear(GL_COLOR_BUFFER_BIT);
        ProcessInput(window);

        GlTry(glUseProgram(shaderProgram));

        GlTry(glDrawArrays(GL_TRIANGLES, 0, 3));

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    while (const auto error = glGetError())
    {
        std::cerr << "OpenGL error: " << error << " in " << std::endl;
    }


    // terminate the glfw
    glfwTerminate();
    return 0;
}
