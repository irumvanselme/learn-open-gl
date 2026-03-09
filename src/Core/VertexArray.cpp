//
// Created by anselme on 06/03/2026.
//


#include <GL/glew.h>
#include "VertexArray.h"
#include "Library.h"

VertexArray::VertexArray()
{
    gl_try(glGenVertexArrays(1, &m_RendererID));
    gl_try(glBindVertexArray(m_RendererID));
}

VertexArray::~VertexArray()
{
    gl_try(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::Bind() const
{
    gl_try(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
    gl_try(glBindVertexArray(0));
}
