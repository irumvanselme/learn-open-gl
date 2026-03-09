//
// Created by anselme on 06/03/2026.
//

#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "Shader.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

class Renderer
{
public:
    void Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const;

    void Clear() const;
};

#endif //OPENGL_RENDERER_H
