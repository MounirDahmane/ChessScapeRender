/**
 * @file ShaderClass.cpp
 * @brief Implementation of shader compilation and cached uniform setters.
 */

#include "ShaderClass.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

MyShader::MyShader(const char *vertexPath, const char *fragmentPath)
    : Path(vertexPath, fragmentPath)
{
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vStream, fStream;
        vStream << vShaderFile.rdbuf();
        fStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vStream.str();
        fragmentCode = fStream.str();
    }
    catch (std::ifstream::failure &e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_READ: " << e.what() << std::endl;
    }

    const char *vCode = vertexCode.c_str();
    const char *fCode = fragmentCode.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vCode, NULL);
    glCompileShader(vertex);
    CheckError(vertex, 1);

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fCode, NULL);
    glCompileShader(fragment);
    CheckError(fragment, 2);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    CheckError(ID, 3);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

MyShader::~MyShader() { glDeleteProgram(ID); }

void MyShader::use() const { glUseProgram(ID); }

// --- Optimized Uniform Logic ---

int MyShader::GetUniformLocation(const std::string &name) const
{
    // If the location is already in the cache, return it immediately.
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    // Otherwise, ask the GPU and store the result.
    int location = glGetUniformLocation(ID, name.c_str());
    if (location == -1)
    {
        std::cerr << "WARNING: Uniform '" << name << "' not found in shader!" << std::endl;
    }

    m_UniformLocationCache[name] = location;
    return location;
}

void MyShader::SetUniform1f(const std::string &name, float value) const
{
    glUniform1f(GetUniformLocation(name), value);
}

void MyShader::SetUniform1i(const std::string &name, int value) const
{
    glUniform1i(GetUniformLocation(name), value);
}

void MyShader::SetUniform3f(const std::string &name, float x, float y, float z) const
{
    glUniform3f(GetUniformLocation(name), x, y, z);
}

void MyShader::SetUniform3f(const std::string &name, const glm::vec3 &value) const
{
    glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
}

void MyShader::SetUniformMat4fv(
    const std::string &name, const glm::mat4 &matrix, bool transpose) const
{
    glUniformMatrix4fv(
        GetUniformLocation(name), 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
}

void MyShader::CheckError(unsigned int shader, int type) const
{
    int success;
    char log[512];
    if (type != 3)
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, log);
            std::cerr << "SHADER_COMPILATION_ERROR: " << (type == 1 ? Path.first : Path.second)
                      << "\n"
                      << log << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 512, NULL, log);
            std::cerr << "SHADER_LINKING_ERROR\n" << log << std::endl;
        }
    }
}