//
// Created by anselme on 08/03/2026.
//

#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/gl.h>

#include "Shader.h"
#include "Library.h"


unsigned int Shader::CreateShader(const std::string &vertexShader, const std::string &fragmentShader)
{
    const unsigned int program = glCreateProgram();
    const unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    if (!vs)
    {
        std::cerr << "Failed to compile vertex shader!" << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    const unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    if (!fs)
    {
        std::cerr << "Failed to compile fragment shader!" << std::endl;
        gl_try(glDeleteShader(vs));
        gl_try(glDeleteProgram(program));
        return 0;
    }

    gl_try(glAttachShader(program, vs));
    gl_try(glAttachShader(program, fs));

    gl_try(glLinkProgram(program));
    gl_try(glValidateProgram(program));

    gl_try(glDeleteShader(vs));
    gl_try(glDeleteShader(fs));

    return program;
}

unsigned int Shader::CompileShader(const unsigned int type, const std::string &source)
{
    // This is the method to dynamically compile the shader into the GPU executable.
    // The first thing to do is to create shader object (As all OpenGL objects we will get an id)
    const unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    gl_try(glShaderSource(id, 1, &src, nullptr));
    gl_try(glCompileShader(id));
    int result;
    gl_try(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    if (!result)
    {
        int length;
        gl_try(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        const auto message = static_cast<char *>(alloca(length * sizeof(char)));
        gl_try(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" <<
                std::endl;
        std::cout << message << std::endl;
        gl_try(glDeleteShader(id));
        return 0;
    }

    // TODO: Error handling
    return id;
}

ShaderProgramSource Shader::ParseShader(const std::string &vertex_file_path)
{
    std::ifstream stream(std::string(PROJECT_ROOT) + "/" + vertex_file_path);

    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };
    std::stringstream ss[2];
    std::string line;
    auto type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        } else
        {
            if (type != ShaderType::NONE)
            {
                ss[static_cast<int>(type)] << line << '\n';
            }
        }
    }

    return {ss[0].str(), ss[1].str()};
}


Shader::Shader(std::string filePath) : m_FilePath(filePath), m_RendererID(0)
{
    auto [vertex_source, fragment_source] = ParseShader(filePath);
    m_RendererID = CreateShader(vertex_source, fragment_source);
}

Shader::~Shader()
{
    gl_try(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
    gl_try(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
    gl_try(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3)
{
    gl_try(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniform1f(const std::string &name, float value)
{
    gl_try(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform1i(const std::string &name, int value)
{
    gl_try(glUniform1i(GetUniformLocation(name), value));
}

int Shader::GetUniformLocation(const std::string &name)
{
    if (!m_UniformLocationCache.contains(name))
    {
        gl_try(const int location = glGetUniformLocation(m_RendererID, name.c_str()));

        if (location == -1)
        {
            std::cerr << "Error: uniform '" << name << "' doesn't exist!" << std::endl;
        }

        m_UniformLocationCache[name] = location;
    }

    return m_UniformLocationCache[name];
}
