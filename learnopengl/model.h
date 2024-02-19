#ifndef MODEL_H
#define MODEL_H

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/glad.h>
#include <learnopengl/mesh.h>
#include <learnopengl/shader.h>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

class Model {
   public:
    // model data
    std::vector<Texture> textures_loaded;  // stores all the textures loaded so far, optimization to
                                           // make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(std::string const &path, const std::vector<std::string> &mesh_names = {},
          bool gamma = false)
        : gammaCorrection(gamma) {
        loadModel(path, mesh_names);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader &shader) const;

   private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in
    // the meshes vector.
    void loadModel(std::string const &path, const std::vector<std::string> &mesh_names);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node
    // and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene,
                     const std::vector<std::string> &mesh_names);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    // checks all material textures of a given type and loads the textures if they're not loaded
    // yet. the required info is returned as a Texture struct.
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type);
};

struct RenderModel {
    Model model;
    glm::vec3 pos;
    glm::vec3 scale;
    float angle;

    void Draw(Shader &shader) const { model.Draw(shader); }
};

#endif