//
// Created by anselme on 06/03/2026.
//

#include <iostream>
#include <GL/glew.h>

#include "Library.h"
#include "Renderer.h"

void Renderer::Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();

    gl_try(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Clear() const
{
    gl_try(glClear(GL_COLOR_BUFFER_BIT));
}
