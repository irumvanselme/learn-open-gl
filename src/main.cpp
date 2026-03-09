//
// Created by anselme on 07/03/2026.
//

#include <iostream>
#include <GL/glew.h>

#include <Application.h>
#include <Library.h>
#include <Shader.h>
#include <VertexArray.h>
#include <VertexBuffer.h>

int main()
{
    constexpr int width = 800;
    constexpr int height = 400;

    constexpr std::string appTitle = "Hello world";
    const Application app(width, height, appTitle);
    app.PrintDebugInfo();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // These are vertices positions 3D Points
    constexpr float positions[] = {
        -.5f, -.5f, 0.0f,   1.0f, 0.0f, 0.0f, // A
         .5f, -.5f, 0.0f,    0.0f, 1.0f, 0.0f, // B
         .0f, .5f, 0.0f,     0.0f, 0.0f, 1.0f // C
    };

    // Load the shader program
    Shader shader("Resources/Shaders/Triangle.glsl");

    // Initialize the vertex Array
    VertexArray va;
    VertexBuffer vb(positions, sizeof(positions));
    gl_try(glEnableVertexAttribArray(0));

    gl_try(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float))));
    gl_try(glEnableVertexAttribArray(1));

    shader.Bind();
    while (!app.IsClosed())
    {
        app.NewFrame();

        shader.Bind();

        // float timeValue = glfwGetTime();
        // float greenValue = sin(timeValue) / 2.0f + 0.5f;
        //
        // shader.SetUniform4f("customColor", 0.0f, greenValue, 0.0f, 1.0f);
        gl_try(glDrawArrays(GL_TRIANGLES, 0, 3));

        app.ClearFrame();
    }

    shader.Unbind();
}
