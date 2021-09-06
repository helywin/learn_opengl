//
// Created by jiang on 2021/9/6.
//

#ifndef LEARN_OPENGL_MESH_HPP
#define LEARN_OPENGL_MESH_HPP

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <assimp/types.h>

class Shader;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> &&vertices, std::vector<unsigned int> &&indices,
         std::vector<Texture> &&textures);

    void draw(const Shader &shader) const;

public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

private:
    unsigned int VAO{}, VBO{}, EBO{};
    void setupMesh();
};


#endif //LEARN_OPENGL_MESH_HPP
