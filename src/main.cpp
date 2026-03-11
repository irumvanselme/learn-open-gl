//
// Created by anselme on 07/03/2026.
//

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Core.h>

#define INITIAL_WIDTH 800
#define INITIAL_HEIGHT 800

using namespace glm;

int main()
{
    const std::string appTitle = "Hello world";
    Application app(INITIAL_WIDTH, INITIAL_HEIGHT, appTitle);

#ifdef ENABLE_DEBUG_LOG
    app.PrintDebugInfo();
#endif

    // Load the shader program
    glEnable(GL_DEPTH_TEST);

    Shader shader("Resources/Shaders/RectangleWithTexture.glsl");

    // These are vertices for a cube
    constexpr float positions[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
    };

    // constexpr unsigned int indices[] = {
    //     0, 1, 3, // first triangle
    //     1, 2, 3 // second triangle
    // };

    VertexArray va;
    va.Bind();

    VertexBuffer vb(positions, sizeof(positions));
    vb.Bind();

    // IndexBuffer ib(indices, 6);
    // ib.Bind();

    VertexBufferLayout vbl;
    vbl.Push<float>(3); // position
    // vbl.Push<float>(3); // color
    vbl.Push<float>(2); // texture coords
    va.AddBuffer(vb, vbl);


    Texture texture0("Resources/Textures/container.jpg");
    Texture texture1("Resources/Textures/awesomeface.png");

    Renderer renderer;

    // Set the background / clear color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // Model matrix
    //   rotates the objects in x-axis by -55 degrees to make it look like a rectangle instead of a square
    auto model_matrix = mat4(1.0f);
    model_matrix = rotate(model_matrix, radians(-55.0f), vec3(1.0f, .0f, .0f));


    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };


    while (!app.IsClosed())
    {
        app.NewFrame();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Let us rotate based on the time to add an animation
        mat4 view_matrix(1.f);
        mat4 projection_matrix(1.f);

        shader.Bind();
        texture0.Bind(0);
        shader.SetUniform1i("texture0", 0);
        texture1.Bind(1);
        shader.SetUniform1i("texture1", 1);

        shader.Bind();



        view_matrix = glm::translate(view_matrix, glm::vec3(0.0f, 0.0f, -3.0f));
        shader.SetUniformMat4f("view_matrix", view_matrix);

        projection_matrix = glm::perspective(glm::radians(45.0f), (float) INITIAL_WIDTH / INITIAL_HEIGHT, 0.1f, 100.0f);

        shader.SetUniformMat4f("projection_matrix", projection_matrix);

        for(unsigned int i = 0; i < 10; i++)
        {
            mat4 model_matrix(1.f);
            model_matrix = glm::translate(model_matrix, cubePositions[i]);
            float angle = 20.0f * i;
            model_matrix = glm::rotate(model_matrix, radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.SetUniformMat4f("model_matrix", model_matrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        va.Bind();
        // renderer.Draw(va, ib, shader);
        app.ClearFrame();
    }

#ifdef ENABLE_DEBUG_LOG
    std::printf("[DEBUG]: Application closed successfully.\n");
#endif

    shader.Unbind();
}
