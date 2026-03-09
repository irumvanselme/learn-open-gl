//
// Created by anselme on 06/03/2026.
//
#include <GL/glew.h>

#include "IndexBuffer.h"
#include "Library.h"

IndexBuffer::IndexBuffer(const unsigned int *data, const unsigned int count)
    : m_Count(count)
{
    gl_try(glGenBuffers(1, &m_RendererID));
    gl_try(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    gl_try(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    gl_try(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::Bind() const
{
    gl_try(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void IndexBuffer::Unbind() const
{
    gl_try(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}