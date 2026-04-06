/**
 * @file mesh.cpp
 * @brief Implementation of Mesh buffer management and rendering logic.
 */

#include "mesh.h"

#include <iostream>

Mesh::Mesh(
    std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    : vertices(std::move(vertices)),
      indices(std::move(indices)),
      textures(std::move(textures)),
      VAO(0),
      VBO(0),
      EBO(0)
{
    setupMesh();
}

Mesh::~Mesh()
{
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
    if (EBO != 0)
        glDeleteBuffers(1, &EBO);
}

// Move Constructor
Mesh::Mesh(Mesh &&other) noexcept
    : vertices(std::move(other.vertices)),
      indices(std::move(other.indices)),
      textures(std::move(other.textures)),
      VAO(other.VAO),
      VBO(other.VBO),
      EBO(other.EBO)
{
    other.VAO = other.VBO = other.EBO = 0;
}

// Move Assignment
Mesh &Mesh::operator=(Mesh &&other) noexcept
{
    if (this != &other)
    {
        // Free current GPU resources
        if (VAO != 0)
            glDeleteVertexArrays(1, &VAO);
        if (VBO != 0)
            glDeleteBuffers(1, &VBO);
        if (EBO != 0)
            glDeleteBuffers(1, &EBO);

        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        textures = std::move(other.textures);

        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;

        other.VAO = other.VBO = other.EBO = 0;
    }
    return *this;
}

void Mesh::Draw(const MyShader &shader, StencilMode stencil) const
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    // OPTIMIZATION: Reduced string allocations in the draw loop
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        const std::string &name = textures[i].type;

        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);
        else if (name == "texture_normal")
            number = std::to_string(normalNr++);
        else if (name == "texture_height")
            number = std::to_string(heightNr++);

        shader.SetUniform1i(name + number, i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // --- Stencil Logic ---
    if (stencil == StencilMode::First)
    {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
    }
    else if (stencil == StencilMode::Second)
    {
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
    }

    // Render Mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Reset State
    if (stencil == StencilMode::Second)
    {
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);
    }
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW);

    // Vertex Attributes
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
    // TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
    // Tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));
    // Bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(
        4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));
    // Bone IDs (Integer attributes need glVertexAttribIPointer)
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, m_BoneIDs));
    // Weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(
        6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_Weights));

    glBindVertexArray(0);
}