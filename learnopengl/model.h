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

#include "shader.h"

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
                     const std::vector<std::string> &mesh_names) {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            // the node object only contains indices to index the actual objects in the scene.
            // the scene contains all the data, node is just to keep stuff organized (like relations
            // between nodes).
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            if (mesh_names.empty() || std::find(mesh_names.begin(), mesh_names.end(),
                                                mesh->mName.C_Str()) != mesh_names.end()) {
                meshes.push_back(processMesh(mesh, scene));
            }
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the
        // children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene, mesh_names);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    // checks all material textures of a given type and loads the textures if they're not loaded
    // yet. the required info is returned as a Texture struct.
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type) {
        std::string typeName = ai_texture_type_to_type.at(type);
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            if (mat->GetTexture(type, i, &str) != AI_SUCCESS) {
                throw std::runtime_error("fail getting texture from material");
            }
            // check if texture was loaded before and if so, continue to next iteration: skip
            // loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++) {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                    textures.push_back(textures_loaded[j]);
                    skip = true;  // a texture with the same filepath has already been loaded,
                                  // continue to next one. (optimization)
                    break;
                }
            }
            if (!skip) {  // if texture hasn't been loaded already, load it
                std::cout << typeName << " texture "
                          << ": " << str.C_Str() << '\n';
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(
                    texture);  // store it as texture loaded for entire model, to ensure we won't
                               // unnecessary load duplicate textures.
            }
        }
        return textures;
    }
};

struct RenderModel {
    Model model;
    glm::vec3 pos;
    glm::vec3 scale;
    float angle;

    void Draw(Shader &shader) const { model.Draw(shader); }
};

#endif