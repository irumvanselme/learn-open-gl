//
// Created by anselme on 08/03/2026.
//

#ifndef LEARN_OPEN_GL_LIBRARY_H
#define LEARN_OPEN_GL_LIBRARY_H

// Enable Debug logging
#define ENABLE_DEBUG_LOG

#include <csignal>

#define ASSERT(x) if (!(x)) raise(SIGTRAP)
#define gl_try(x) gl_clear_error();\
x;\
ASSERT(gl_check_error(#x ,__FILE__, __LINE__))

void gl_clear_error();

bool gl_check_error(const char *function, const char *file, int line);

#endif
