//
// Created by anselme on 06/03/2026.
//

#ifndef OPENGL_VERTEX_BUFFER_LAYOUT_H
#define OPENGL_VERTEX_BUFFER_LAYOUT_H

#include <vector>
#include <glad/gl.h>

struct VertexBufferElement
{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int GetSizeOfType(const unsigned int type)
    {
        switch (type)
        {
            case GL_FLOAT: return 4;
            case GL_UNSIGNED_BYTE: return 1;
            case GL_UNSIGNED_INT: return 4;
            default: return 0;
        }

        return 0;
    }
};

class VertexBufferLayout
{
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;

public:
    VertexBufferLayout() : m_Stride(0)
    {
    }

    template<typename T>
    void Push(const unsigned int count)
    {
        static_assert(sizeof(T) == 0, "Unsupported type for Push");
    }

    inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
    inline unsigned int GetStride() const { return m_Stride; }
};

// Template specializations must be outside the class definition
template<>
inline void VertexBufferLayout::Push<float>(const unsigned int count)
{
    m_Elements.push_back({
        GL_FLOAT,
        count,
        GL_FALSE
    });
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
}

template<>
inline void VertexBufferLayout::Push<unsigned int>(const unsigned int count)
{
    m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
}

template<>
inline void VertexBufferLayout::Push<unsigned char>(const unsigned int count)
{
    m_Elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}

#endif //OPENGL_VERTEX_BUFFER_LAYOUT_H
