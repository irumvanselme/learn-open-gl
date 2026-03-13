//
// Created by anselme on 13/03/2026.
//

#include <iostream>
#include <ostream>
#include <csignal>
#include <tuple>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define ASSERT(x) if (!(x)) raise(SIGTRAP)
#define GlTry(x) gl_clear_error();\
x;\
ASSERT(gl_check_error(#x ,__FILE__, __LINE__))

void gl_clear_error()
{
    while (glGetError() != GL_NO_ERROR)
    {
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


auto VERTEX_SHADER = R"(
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 inTextureCord;

out vec2 outTextureCord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position.xyz, 1.0f);
    outTextureCord = inTextureCord;
}
)";

auto FRAGMENT_SHADER = R"(
#version 330 core

in vec2 outTextureCord;
out vec4 FragColor;

uniform sampler2D containerTexture;

void main()
{
    FragColor = texture(containerTexture, outTextureCord);
}
)";


void ProcessInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

auto CompileShader(const char *shaderSource, const GLenum shaderType)
{
    int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);
    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        const auto message = static_cast<char *>(alloca(length * sizeof(char)));
        glGetShaderInfoLog(shader, length, &length, message);
        std::cerr << "Failed to compile shader: " << message << std::endl;
        glDeleteShader(shader);
    }

    return shader;
}

auto CreateShaderProgram()
{
    int program = glCreateProgram();
    auto vertexShader = CompileShader(VERTEX_SHADER, GL_VERTEX_SHADER);
    auto fragmentShader = CompileShader(FRAGMENT_SHADER, GL_FRAGMENT_SHADER);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    glUseProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

auto LoadTexture(const char *path)
{
    stbi_set_flip_vertically_on_load(true);
    auto full_path = std::string(PROJECT_ROOT) + "/" + path;
    int channels_in_file;
    int width, height;
    auto file = stbi_load(full_path.c_str(), &width, &height, &channels_in_file, 4);

    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    GlTry(glBindTexture(GL_TEXTURE_2D, texture_id));
    GlTry(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GlTry(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GlTry(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GlTry(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GlTry(glTexImage2D(
        GL_TEXTURE_2D,

        0,

        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        file));
    GlTry(glBindTexture(GL_TEXTURE_2D, texture_id));
    GlTry(glGenerateMipmap(GL_TEXTURE_2D));
    stbi_image_free(file);
    return texture_id;
}


std::tuple<std::array<float, 40>, std::array<unsigned int, 36> > ConstructCubeMesh(
    float o_x, float o_y, float o_z, float height, float width, float length)
{
    auto vertices = std::array{
        o_x, o_y, o_z, 0.0f, 0.0f,
        o_z, o_y + height, o_z, 0.0f, 1.0f,
        o_x + length, o_y + height, o_z, 1.0f, 1.0f,
        o_x + length, o_y, o_z, 1.0f, 0.0f,

        o_x, o_y, o_z + width, 1.0f, 0.0f,
        o_x, o_y + height, o_z + width, 1.0f, 1.0f,
        o_x + length, o_y + height, o_z + width, 0.0f, 1.0f,
        o_x + length, o_y, o_z + width, 0.0f, 0.0f,
    };

    auto indices = std::array<unsigned int, 36>{
        0, 1, 2,
        0, 2, 3,

        3, 2, 6,
        3, 6, 7,

        0, 1, 5,
        0, 5, 4,

        4, 5, 6,
        4, 6, 7,


        1, 2, 6,
        1, 6, 5,

        0, 3, 7,
        0, 7, 4
    };

    return std::make_tuple(vertices, indices);
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to init glfw" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(1000, 1000, "Lesson -> Cube", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cout << "Failed to init glad" << std::endl;
        glfwTerminate();
        return 1;
    }

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    const auto program = CreateShaderProgram();
    glUseProgram(program);

    const auto texture = LoadTexture("Resources/Textures/container.jpg");

    const auto [vertices, indices] = ConstructCubeMesh(
        -.5f, -.5f, 0.0f,
        1.0f, 1.0f, 1.0f);

    unsigned int vao;
    GlTry(glGenVertexArrays(1, &vao));
    GlTry(glBindVertexArray(vao));

    unsigned int vbo;
    GlTry(glGenBuffers(1, &vbo));
    GlTry(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GlTry(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW));

    GlTry(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0));
    GlTry(glEnableVertexAttribArray(0));

    GlTry(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float))));
    GlTry(glEnableVertexAttribArray(1));

    unsigned int ebo;
    GlTry(glGenBuffers(1, &ebo));
    GlTry(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    GlTry(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW));

    // Set the texture uniform
    GlTry(glUniform1i(glGetUniformLocation(program, "containerTexture"), 0));
    GlTry(glActiveTexture(GL_TEXTURE0));
    GlTry(glBindTexture(GL_TEXTURE_2D, texture));

    GlTry(glClearColor(1.f, 0.f, 0.f, 1.f));

    auto model = glm::mat4(1.0);
    GlTry(glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model)));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1000.0f / 1000.0f, 0.1f, 100.0f);
    GlTry(glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection)));

    while (!glfwWindowShouldClose(window))
    {
        GlTry(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        ProcessInput(window);
        GlTry(glBindVertexArray(vao));
        GlTry(glUseProgram(program));

        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;

        glm::mat4 view;
        view = glm::lookAt(glm::vec3(camX, camX, camZ), // Camera positon
                     glm::vec3(0.0f, 0.0f, 0.0f),  // target [mostly constant because we would liket o be looking at the center]
                     glm::vec3(0.0f, 1.0f, 0.0f)); // up vector [constant]
        // view = glm::translate(view, glm::vec3(0.0f, -1.0f, 0.0f));
        // float angle = fmod(glfwGetTime(), 1.0) * 2.0f * M_PI; // 0 to 2π
        // view = glm::rotate(view, angle, glm::vec3(1.f, 1.f, 1.f));
        GlTry(glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view)));

        GlTry(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0));
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
