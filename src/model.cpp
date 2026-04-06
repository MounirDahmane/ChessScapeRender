/**
 * @file model.cpp
 * @brief Implementation of 3D model loading and processing using Assimp.
 */

#include "model.h"

#include "../dependencies/stb-master/stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem> // C++17 path handling
#include <iostream>

namespace fs = std::filesystem;

Model::Model(const std::string &path, bool gamma)
    : gammaCorrection(gamma)
{
    loadModel(path);
}

void Model::Draw(const MyShader &shader, StencilMode stencil) const
{
    for (const auto &mesh : meshes)
    {
        mesh.Draw(shader, stencil);
    }
}

void Model::loadModel(const std::string &path)
{
    Assimp::Importer importer;
    // Optimization: These flags are standard for real-time rendering
    const aiScene *scene = importer.ReadFile(
        path,
        aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace |
            aiProcess_JoinIdenticalVertices); // Optimization: merges duplicate vertices

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    directory = fs::path(path).parent_path().string();

    std::cout << "SUCCESS: Loaded model [" << path << "] | Meshes: " << scene->mNumMeshes
              << std::endl;

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // Process each mesh at this node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // Recursively process children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // OPTIMIZATION: Pre-allocate memory to prevent multiple reallocations
    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // Position
        vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        // Normals
        if (mesh->HasNormals())
        {
            vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        }

        // Texture Coordinates / Tangents
        if (mesh->mTextureCoords[0])
        {
            vertex.TexCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
            vertex.Tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
            vertex.Bitangent = {
                mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Process materials
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    auto specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

std::vector<Texture> Model::loadMaterialTextures(
    aiMaterial *mat, aiTextureType type, const std::string &typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        // OPTIMIZATION: Check if texture was loaded previously
        bool skip = false;
        for (const auto &loadedTex : textures_loaded)
        {
            if (std::strcmp(loadedTex.path.data(), str.C_Str()) == 0)
            {
                textures.push_back(loadedTex);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
    fs::path fullPath = fs::path(directory) / path;
    std::string filename = fullPath.string();

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = (nrComponents == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "TEXTURE::LOAD_ERROR: " << filename << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}