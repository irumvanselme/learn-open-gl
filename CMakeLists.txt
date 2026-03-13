cmake_minimum_required(VERSION 4.1)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 20)

# Dependency GLFW
include(FetchContent)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# Fetch GLM (OpenGL Mathematics library)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)

# Fetch GLAD (OpenGL loader)
FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG glad2
)
FetchContent_MakeAvailable(glad)

# Generate GLAD for OpenGL 4.6 Core Profile
add_subdirectory("${glad_SOURCE_DIR}/cmake" glad_cmake)
glad_add_library(glad_gl_core_46 REPRODUCIBLE API gl:core=4.6)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

find_package(OpenGL REQUIRED)

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
add_executable(_Practice src/Practice/main.cpp)

target_compile_definitions(LearnOpenGL
        PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)

set(DEPENDENCIES
        glfw
        glm::glm
        glad_gl_core_46
        stb
        OpenGL::GL)

target_link_libraries(LearnOpenGL
        PRIVATE
        ${DEPENDENCIES}
)

target_link_libraries(_Practice
        PRIVATE
        ${DEPENDENCIES}
)

function (setup_lesson lesson_name)
    add_executable(Lesson_${lesson_name} src/Lessons/${lesson_name}/main.cpp)

    target_compile_definitions(Lesson_${lesson_name}
            PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
    )

    target_link_libraries(Lesson_${lesson_name}
            PRIVATE
            ${DEPENDENCIES}
    )

    target_link_libraries(Lesson_${lesson_name}
            PRIVATE
            ${DEPENDENCIES}
    )

endfunction()

setup_lesson(triangle)
setup_lesson(rectangle)
setup_lesson(circle)
setup_lesson(cube)

# Platform-specific libraries
if(APPLE)
    target_link_libraries(LearnOpenGL
            PRIVATE
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
    )
endif()
