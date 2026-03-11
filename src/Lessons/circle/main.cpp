//
// Created by anselme on 13/03/2026.
//

#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

auto VERTEX_SHADER = R"glsl(
#version 330 core

layout(location = 0) in vec2 position;

void main() {
    gl_Position = vec4(position.xy, 1.0f, 1.0f);
}
)glsl";

auto FRAGMENT_SHADER = R"glsl(
#version 330 core

out vec4 FragColor;

void main() {
    FragColor = vec4(0.2, 0.7, 1.0, 1.0);
}

)glsl";

int CreateShaderProgram()
{
    int shaderProgram = glCreateProgram();

    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VERTEX_SHADER, nullptr);
    glCompileShader(vertexShader);
    int result;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        int length;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &length);
        const auto message = static_cast<char *>(alloca(length * sizeof(char)));
        glGetShaderInfoLog(vertexShader, length, &length, message);
        std::cerr << "Failed to compile vertex shader!" << std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(vertexShader);
        return 0;
    }

    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FRAGMENT_SHADER, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        int length;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &length);
        const auto message = static_cast<char *>(alloca(length * sizeof(char)));
        glGetShaderInfoLog(fragmentShader, length, &length, message);
        std::cerr << "Failed to compile fragment shader!" << std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(fragmentShader);
        return 0;
    }

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);

    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void generateCircleVertices(float cx, float cy, float radius,
                            int segments,
                            float* vertices, int* vertexCount)
{
    int idx = 0;

    // Centre vertex
    vertices[idx++] = cx;
    vertices[idx++] = cy;

    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        vertices[idx++] = cx + radius * cosf(angle);
        vertices[idx++] = cy + radius * sinf(angle);
    }

    *vertexCount = segments + 2;   // centre + (segments+1) rim vertices
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to init glfw" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Lesson -> Circle", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cerr << "Failed to init glad" << std::endl;
        glfwTerminate();
        return 1;
    }

    const int   SEGMENTS = 10;
    const int   MAX_VERTS = (SEGMENTS + 2) * 2;  // 2 floats per vertex
    float       vertices[MAX_VERTS];
    int         vertexCount = 0;

    generateCircleVertices(0.0f, 0.0f,   // centre
                           0.5f,          // radius (NDC)
                           SEGMENTS,
                           vertices, &vertexCount);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    auto shader = CreateShaderProgram();
    if (!shader)
    {
        std::cerr << "Failed to create shader program" << std::endl;
        glfwTerminate();
        return 1;
    }

    glUseProgram(shader);

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                   vertexCount * 2 * sizeof(float),
                   vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);

    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(vao);
        glUseProgram(shader);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    while (const auto error = glGetError())
    {
        std::cerr << "OpenGL error: " << error << std::endl;
    }

    glfwTerminate();
}
