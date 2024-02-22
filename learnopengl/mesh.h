#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>  // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <string>
#include <vector>

#include "shader.h"

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    // bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    // weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

struct Material {
    std::string name;
    glm::vec3 color_ambient;
    glm::vec3 color_diffuse;
    glm::vec3 color_specular;
    float shininess = 0;
    float dissolve = 1.0;
    float refracti = 1.0;
};

extern std::map<aiTextureType, std::string> ai_texture_type_to_type;

class Mesh {
   public:
    // constructor
    Mesh(const std::string &name, std::vector<Vertex> vertices, std::vector<unsigned int> indices,
         std::multimap<std::string, Texture> textures, Material material);

    // render the mesh
    void Draw(Shader &shader) const;

    unsigned int getVAO() const { return VAO; }
    const std::vector<unsigned int> &getindices() const { return indices; }

    bool isTransparent() const { return material.dissolve != 1.0; }

    static std::map<std::string, Texture> dummy_textures;
    static void loadDummyTextures();

   private:
    // mesh Data
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::multimap<std::string, Texture> textures;
    Material material;
    unsigned int VAO;

    // render data
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
};

unsigned int TextureFromFile(std::string_view filename, const std::string &directory);

#endif