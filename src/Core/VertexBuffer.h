//
// Created by anselme on 06/03/2026.
//

#ifndef OPENGL_VERTEX_BUFFER_H
#define OPENGL_VERTEX_BUFFER_H

class VertexBuffer
{
private:
    unsigned int m_RendererID{};

public:
    VertexBuffer(const void *data, unsigned int dataSize);

    ~VertexBuffer();

    void Bind() const;

    void Unbind() const;
};


#endif //OPENGL_VERTEX_BUFFER_H
