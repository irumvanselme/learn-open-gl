//
// Created by anselme on 06/03/2026.
//
#pragma once

#ifndef OPENGL_VERTEX_ARRAY_H
#define OPENGL_VERTEX_ARRAY_H
#include "VertexBuffer.h"

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

    void AddBuffer(const VertexBuffer & vb, const VertexBufferLayout & vbl);
};

#endif //OPENGL_VERTEX_ARRAY_H
