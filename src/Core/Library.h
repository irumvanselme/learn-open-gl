//
// Created by anselme on 08/03/2026.
//

#ifndef TWODSHAPES_LIBRARY_H
#define TWODSHAPES_LIBRARY_H

#include <csignal>

#define ASSERT(x) if (!(x)) raise(SIGTRAP)
#define gl_try(x) gl_clear_error();\
x;\
ASSERT(gl_check_error(#x ,__FILE__, __LINE__))

void gl_clear_error();

bool gl_check_error(const char *function, const char *file, int line);

#endif //TWODSHAPES_LIBRARY_H
