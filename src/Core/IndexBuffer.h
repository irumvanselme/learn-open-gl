//
// Created by anselme on 06/03/2026.
//

#ifndef OPENGL_INDEX_BUFFER_H
#define OPENGL_INDEX_BUFFER_H

class IndexBuffer
{
private:
    unsigned int m_RendererID{};
    unsigned int m_Count;

public:
    IndexBuffer(const unsigned int *data, unsigned int count);

    ~IndexBuffer();

    void Bind() const;

    void Unbind() const;

    inline unsigned int GetCount() const { return m_Count; }
};


#endif //OPENGL_INDEX_BUFFER_H