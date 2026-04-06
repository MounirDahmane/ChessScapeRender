/**
 * @file model.h
 * @brief Defines the Model class for loading and managing 3D assets.
 */

#pragma once

#include "ShaderClass.h"
#include "mesh.h"

#include <string>
#include <vector>

// Include these so the compiler knows the full definitions of the Assimp types
#include <assimp/material.h>
#include <assimp/scene.h>

class Model
{
public:
    Model(const std::string &path, bool gamma = false);

    void Draw(const MyShader &shader, StencilMode stencil) const;

private:
    // Model Data
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    // Loading Functions
    void loadModel(const std::string &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    // Use the exact Assimp enum type here
    std::vector<Texture> loadMaterialTextures(
        aiMaterial *mat, aiTextureType type, const std::string &typeName);
};

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);