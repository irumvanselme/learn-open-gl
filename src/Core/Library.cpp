//
// Created by anselme on 08/03/2026.
//

#include <iostream>
#include <GL/glew.h>
#include "Library.h"

void gl_clear_error()
{
    while (glGetError() != GL_NO_ERROR)
    {
        std::cout << "OpenGL error: " << glGetError() << std::endl;
    }
}

bool gl_check_error(const char *function, const char *file, int line)
{
    while (const auto error = glGetError())
    {
        std::cout << "OpenGL error: " << error << " in " << function << " at " << file << ":" << line << std::endl;
        return false;
    }

    return true;
}
