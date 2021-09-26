//
// Created by jiang on 2021/9/6.
//

#include "Model.hpp"
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <stb_image.h>

#include "Mesh.hpp"
#include "Shader.hpp"

unsigned int textureFromFile(const std::string &path);

class ModelPrivate
{
public:
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> texturesLoaded;

    void loadModel(const std::string &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                              const std::string &typeName);
};

void ModelPrivate::loadModel(const std::string &path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
    std::cout << scene->mNumMeshes << " meshes, "
              << scene->mNumLights << " lights, "
              << scene->mNumCameras << " cameras, "
              << scene->mNumAnimations << " animations, "
              << scene->mNumMaterials << " materials, "
              << std::endl;
}

void ModelPrivate::processNode(aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.emplace_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh ModelPrivate::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    // 读取顶点数据
    std::cout << "vertices (" << mesh->mName.C_Str() << "): " << mesh->mNumVertices << std::endl;
    std::cout << "faces (" << mesh->mName.C_Str() << "): " << mesh->mNumFaces << std::endl;
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0]) {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.texCoords.x = 0;
            vertex.texCoords.y = 0;
        }
        vertices.emplace_back(vertex);
    }

    // 读取EBO数据
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.emplace_back(face.mIndices[j]);
        }
    }

    Material material;
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
        aiColor3D color;
        if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == aiReturn_SUCCESS) {
            material.ambient.x = color.r;
            material.ambient.y = color.g;
            material.ambient.z = color.b;
        }
        if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS) {
            material.diffuse.x = color.r;
            material.diffuse.y = color.g;
            material.diffuse.z = color.b;
        }
        std::vector<Texture> diffuseMaps = loadMaterialTextures(mat, aiTextureType_DIFFUSE,
                                                                "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(mat, aiTextureType_SPECULAR,
                                                                 "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    return Mesh{std::move(vertices),
                std::move(indices),
                std::move(material)};
}

std::vector<Texture> ModelPrivate::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                        const std::string &typeName)
{
    std::vector<Texture> textures;
    unsigned int count = mat->GetTextureCount(type);
    for (unsigned int i = 0; i < count; ++i) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = std::any_of(texturesLoaded.begin(), texturesLoaded.end(),
                                [&str](const Texture &iter) {
                                    return iter.path == std::string(str.C_Str());
                                });
        if (!skip) {
            Texture texture;
            std::string file(str.C_Str());
            std::for_each(file.begin(), file.end(), [](char &c) {
                if (c == '\\') {
                    c = '/';
                }
            });
            texture.id = textureFromFile(directory + "/" + file);
            texture.type = typeName;
            texture.path = file;
            textures.push_back(texture);
            texturesLoaded.push_back(texture);
        }
    }
    return textures;
}

Model::Model(const std::string &path) :
        d(new ModelPrivate)
{
    d->loadModel(path);
}

void Model::draw(const Shader &shader) const
{
    for (const auto &mesh : d->meshes) {
        mesh.draw(shader);
    }
}

Model::~Model()
{
    delete d;
}

unsigned int textureFromFile(const std::string &path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = GL_BLUE;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}