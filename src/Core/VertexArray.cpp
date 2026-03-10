//
// Created by anselme on 06/03/2026.
//

#include <cstdint>
#include <iostream>
#include <glad/gl.h>

#include "Library.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"

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

void VertexArray::AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout)
{
    Bind();
    vb.Bind();
    const auto elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); ++i)
    {
        const auto &[type, count, normalized] = elements[i];

        gl_try(glVertexAttribPointer(
            i,
            count,
            type,
            normalized,
            layout.GetStride(),
            reinterpret_cast<void *>(static_cast<std::uintptr_t>(offset))));
        gl_try(glEnableVertexAttribArray(i));

        offset += count * VertexBufferElement::GetSizeOfType(type);
    }
}
