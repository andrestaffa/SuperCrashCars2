#pragma once

#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>
#include <string>

#include "ShaderProgram.h"
#include "Log.h"

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct TexMesh {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {

public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TexMesh> textures);

    void draw(glm::mat4& TM, ShaderProgram& shader, int renderMode);

    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<TexMesh> m_textures;

private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();

};