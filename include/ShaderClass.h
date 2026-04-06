/**
 * @file ShaderClass.h
 * @brief Handles shader compilation, linking, and uniform management.
 */

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class MyShader
{
public:
    unsigned int ID;
    std::pair<std::string, std::string> Path;

    /**
     * @brief Loads and compiles vertex and fragment shaders.
     * @param vertexPath Path to the .vs or .txt vertex shader file.
     * @param fragmentPath Path to the .fs or .txt fragment shader file.
     */
    MyShader(const char *vertexPath, const char *fragmentPath);
    ~MyShader();

    // Rule of Three: Shaders manage a GPU resource (ID), so copying is prohibited.
    MyShader(const MyShader &) = delete;
    MyShader &operator=(const MyShader &) = delete;

    void use() const;

    // --- Uniform Setters (Optimized with caching) ---
    void SetUniform1f(const std::string &name, float value) const;
    void SetUniform1i(const std::string &name, int value) const;
    void SetUniform3f(const std::string &name, float x, float y, float z) const;
    void SetUniform3f(const std::string &name, const glm::vec3 &value) const;
    void SetUniformMat4fv(
        const std::string &name, const glm::mat4 &matrix, bool transpose = false) const;

private:
    // Optimization: Caches uniform locations to avoid expensive glGetUniformLocation calls.
    mutable std::unordered_map<std::string, int> m_UniformLocationCache;

    int GetUniformLocation(const std::string &name) const;
    void CheckError(unsigned int shader, int type) const;
};