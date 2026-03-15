//
// Created by anselme on 13/03/2026.
//

#include <iostream>
#include <ostream>
#include <csignal>
#include <tuple>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
        o_x, o_y + height, o_z, 0.0f, 1.0f,
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

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


int main()
{
    std::cout << sin(2 * M_PI) << "-" << M_PI << std::endl;
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

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lesson -> Cube", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

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
        -.5f, -.5f, -2.0f,
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

    GlTry(glClearColor(0.2f, 0.3f, 0.3f, 1.f));

    auto model = glm::mat4(1.0);
    GlTry(glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model)));
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    GlTry(glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection)));

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        GlTry(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        ProcessInput(window);
        processInput(window);
        GlTry(glBindVertexArray(vao));
        GlTry(glUseProgram(program));

        glm::mat4 view = camera.GetViewMatrix();
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


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}