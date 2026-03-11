//
// Created by anselme on 07/03/2026.
//

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Core.h>

#define INITIAL_WIDTH 800
#define INITIAL_HEIGHT 800

int main()
{
    const std::string appTitle = "Hello world";
    Application app(INITIAL_WIDTH, INITIAL_HEIGHT, appTitle);

    #ifdef ENABLE_DEBUG_LOG
        app.PrintDebugInfo();
    #endif

    // Load the shader program
    Shader shader("Resources/Shaders/RectangleWithTexture.glsl");

    // These are vertices positions 3D Points
    constexpr float positions[] = {
        // positions          // colors           // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
    };

    constexpr unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    VertexArray va;
    va.Bind();

    VertexBuffer vb(positions, sizeof(positions));
    vb.Bind();

    IndexBuffer ib(indices, 6);
    ib.Bind();

    VertexBufferLayout vbl;
    vbl.Push<float>(3); // position
    vbl.Push<float>(3); // color
    vbl.Push<float>(2); // texture coords
    va.AddBuffer(vb, vbl);

    shader.Bind();

    Texture texture0("Resources/Textures/container.jpg");
    Texture texture1("Resources/Textures/awesomeface.png");

    texture0.Bind(0);
    shader.SetUniform1i("texture0", 0);
    texture1.Bind(1);
    shader.SetUniform1i("texture1", 1);

    Renderer renderer;

    // Set the background / clear color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    while (!app.IsClosed())
    {
        app.NewFrame();
        // Let us rotate based on the time to add an animation
        auto rotation_vector = glm::vec3(0.0f, 0.0f, 1.0f);
        auto transformation_vector = glm::mat4(1.0f);
        transformation_vector = glm::rotate(transformation_vector, (float)glfwGetTime(), rotation_vector);
        transformation_vector = glm::translate(transformation_vector, glm::vec3(0.5f, -0.5f, 0.0f));
        shader.SetUniformMat4f("transformation_vector", transformation_vector);
        shader.Bind();
        renderer.Draw(va, ib, shader);
        app.ClearFrame();
    }

    shader.Unbind();
}
