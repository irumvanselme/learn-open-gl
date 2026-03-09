cmake_minimum_required(VERSION 4.1)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 20)

# Dependency GLFW
set(GLFW_ROOT ${CMAKE_SOURCE_DIR}/Dependencies/GLFW)
include_directories(${GLFW_ROOT}/include)

# Dependency: GLEW
set(GLEW_ROOT ${CMAKE_SOURCE_DIR}/Dependencies/GLEW)
include_directories(${GLEW_ROOT}/include)

# Dependency: STB
set(STB_ROOT ${CMAKE_SOURCE_DIR}/Dependencies/STB)
include_directories(${STB_ROOT}/include)

find_package(OpenGL REQUIRED)

# Common libraries
set(COMMON_LIBS
        ${GLFW_ROOT}/lib-arm64/libglfw3.a
        ${GLEW_ROOT}/lib/libGLEW.a
        OpenGL::GL
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
)

# Core Modules
set(CORE_MODULES
        src/Core/Core.h
        src/Core/VertexBufferLayout.h

        src/Core/Application.h
        src/Core/Application.cpp

        src/Core/Renderer.h
        src/Core/Renderer.cpp

        src/Core/Shader.h
        src/Core/Shader.cpp

        src/Core/Library.h
        src/Core/Library.cpp

        src/Core/VertexArray.h
        src/Core/VertexArray.cpp

        src/Core/VertexBuffer.h
        src/Core/VertexBuffer.cpp

        src/Core/IndexBuffer.h
        src/Core/IndexBuffer.cpp

        src/Core/Texture.cpp
        src/Core/Texture.h
)

# Include the Core module
include_directories(${CMAKE_SOURCE_DIR}/src/Core)

# Executables
add_executable(LearnOpenGL ${CORE_MODULES} src/main.cpp)

target_compile_definitions(LearnOpenGL
        PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)


target_link_libraries(LearnOpenGL ${COMMON_LIBS})

