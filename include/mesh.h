/**
 * @file mesh.h
 * @brief Defines the Mesh class and associated structures for 3D rendering.
 */

#pragma once

#include "ShaderClass.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#define MAX_BONE_INFLUENCE 4

/** @brief Modes for stencil buffer testing, used for outlining or selection. */
enum class StencilMode
{
    None,
    First,
    Second
};

/** @brief Layout of a single vertex in GPU memory. */
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

/** @brief Metadata for a texture loaded on the GPU. */
struct Texture
{
    unsigned int id;
    std::string type; ///< e.g., "texture_diffuse"
    std::string path;
};

class Mesh
{
public:
    // --- Public Members ---
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO;

    /** @brief Constructs a mesh and initializes GPU buffers. */
    Mesh(
        std::vector<Vertex> vertices,
        std::vector<unsigned int> indices,
        std::vector<Texture> textures);

    // --- Rule of Five ---
    ~Mesh();
    Mesh(const Mesh &) = delete; ///< Copying meshes is disabled to prevent GPU ID conflicts.
    Mesh &operator=(const Mesh &) = delete;
    Mesh(Mesh &&other) noexcept; ///< Move constructor transfers GPU ownership.
    Mesh &operator=(Mesh &&other) noexcept;

    /** * @brief Renders the mesh using the provided shader.
     * @param shader The active shader program.
     * @param stencil Stencil logic to apply for this draw call.
     */
    void Draw(const MyShader &shader, StencilMode stencil) const;

private:
    unsigned int VBO, EBO;

    /** @brief Generates VAO, VBO, and EBO; configures vertex attributes. */
    void setupMesh();
};