//
// Created by anselme on 06/03/2026.
//
#pragma once

#ifndef OPENGL_VERTEX_ARRAY_H
#define OPENGL_VERTEX_ARRAY_H

class VertexBufferLayout;

class VertexArray
{
private:
    unsigned int m_RendererID;

public:
    VertexArray();

    ~VertexArray();

    void Bind() const;

    void Unbind() const;
};

#endif //OPENGL_VERTEX_ARRAY_H
