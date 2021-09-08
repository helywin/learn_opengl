//
// Created by jiang on 2021/9/6.
//

#include "Mesh.hpp"
#include <glad/glad.h>
#include "Shader.hpp"

Mesh::Mesh(std::vector<Vertex> &&vertices, std::vector<unsigned int> &&indices,
           Material &&material) :
        vertices(std::forward<decltype(vertices)>(vertices)),
        indices(std::forward<decltype(indices)>(indices)),
        material(std::forward<Material>(material))
{
    setupMesh();
}

void Mesh::draw(const Shader &shader) const
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    shader.setVec3("material.ambient", material.ambient);
    shader.setVec3("material.diffuse", material.diffuse);
    shader.setVec3("material.specular", material.specular);
    shader.setFloat("material.shininess", material.shininess);
    for (unsigned int i = 0; i < material.textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string name = std::string("material.") + material.textures[i].type;
        if (material.textures[i].type == "texture_diffuse")
            name += std::to_string(diffuseNr++);
        else if (material.textures[i].type == "texture_specular")
            name += std::to_string(specularNr++);
        shader.setInt(name, i); // todo ?
        glBindTexture(GL_TEXTURE_2D, material.textures[i].id);
    }
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0],
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *) offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *) offsetof(Vertex, texCoords));

    // 清空绑定
    glBindVertexArray(0);
}
