//
// Created by anselme on 08/03/2026.
//

#ifndef TWODSHAPES_SHADER_H
#define TWODSHAPES_SHADER_H

#include <string>
#include <unordered_map>

struct ShaderProgramSource
{
    std::string vertex_source;
    std::string fragment_source;
};

class Shader
{
private:
    std::string m_FilePath;
    unsigned int m_RendererID;

    // caching for uniforms;
    std::unordered_map<std::string, int> m_UniformLocationCache;

public:
    Shader(std::string filePath);

    ~Shader();

    void Bind() const;

    void Unbind() const;

    void SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3);

    void SetUniform1f(const std::string &name, float value);

    void SetUniform1i(const std::string &name, int value);

private:
    int GetUniformLocation(const std::string &name);

    unsigned int CompileShader(unsigned int type, const std::string &source);

    static ShaderProgramSource ParseShader(const std::string &vertex_file_path);

    unsigned int CreateShader(const std::string &vertexShader, const std::string &fragmentShader);
};


#endif //TWODSHAPES_SHADER_H
