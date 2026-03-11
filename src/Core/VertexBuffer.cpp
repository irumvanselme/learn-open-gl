//
// Created by anselme on 06/03/2026.
//
#include <glad/gl.h>
#include "VertexBuffer.h"
#include "Library.h"

VertexBuffer::VertexBuffer(const void *data, const unsigned int dataSize)
{

    // We need to send our data to the GPU, so that it will be extremely fast to read our vertices,
    // Soo we are going to create this memory using a Vertex Buffer Object (VBO) and send our vertices data to it.
    // As any other OpenGL object we need to generate it first.
    // Since we are creating one object we are going to generate one object and store its id in a variable using deep copy.
    // Or assign value by reference.

    // The above step is usually summarized into <Allocate memory for the object and add data into the memory>
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

    /// GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
    /// GL_STATIC_DRAW: the data is set only once and used many times.
    /// GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    /// This is to help the OpenGL to know where to store the data.
    /// Depending on the usage, some places offer faster writes or reads respectivelly.
    /// So is the benefit we get from telling OpenGL how we are going to use the data, it can optimize the memory management and access to the data.
    glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);

    // glVertexAttribPointer(
    //     // The first parameter specifies which vertex attribute we want to configure.
    //     // Remember that we specified the location of the position vertex attribute in the vertex shader with layout (location = 0).
    //     // This sets the location of the vertex attribute to 0 and since we want to pass data to this vertex attribute, we pass in 0.
    //     0, // layout (location = 0) in vec3 <variable name>;
    //
    //     // The size of the vertex attribute, since each vertex in our case has 3 components (x, y and z) we pass in 3.
    //     3, // one row in the positions,
    //     GL_FLOAT,
    //     GL_FALSE,
    //     // The fifth argument is known as the ` and tells us the space between consecutive vertex attributes.
    //     // Since the next set of position data is located exactly 3 times the size of a float away we specify that value as the stride.
    //     // Note that since we know that the array is tightly packed (there is no space between the next vertex attribute value)
    //     // we could've also specified the stride as 0 to let OpenGL determine the stride (this only works when values are tightly packed).
    //     // Whenever we have more vertex attributes we have to carefully define the spacing between each vertex attribute but we'll get to see more examples of that later on.
    //     6 * sizeof(float),
    //     (void *) 0
    // );

}

VertexBuffer::~VertexBuffer()
{
    gl_try(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
    gl_try(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const
{
    gl_try(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
