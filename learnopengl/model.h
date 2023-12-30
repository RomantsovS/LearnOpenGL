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
#include <vector>

unsigned int TextureFromFile(const char *path, const std::string &directory);

class Model {
   public:
    // model data
    std::vector<Texture> textures_loaded;  // stores all the textures loaded so far, optimization to
                                           // make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(std::string const &path, bool gamma = false) : gammaCorrection(gamma) { loadModel(path); }

    // draws the model, and thus all its meshes
    void Draw(Shader &shader) const {
        for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].Draw(shader);
    }

   private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in
    // the meshes vector.
    void loadModel(std::string const &path) {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene *scene =
            importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                        aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
            !scene->mRootNode)  // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            throw std::runtime_error("ERROR::ASSIMP");
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);

        glm::vec3 min(0), max(0);
        for (size_t i = 0; i < scene->mNumMeshes; ++i) {
            const auto *mesh = scene->mMeshes[i];
            std::cout << "mesh " << i << ": verts: " << mesh->mNumVertices << '\n';
            for (size_t j = 0; j < mesh->mNumVertices; ++j) {
                min.x = std::min(min.x, mesh->mVertices[j].x);
                min.y = std::min(min.y, mesh->mVertices[j].y);
                min.z = std::min(min.z, mesh->mVertices[j].z);

                max.x = std::max(max.x, mesh->mVertices[j].x);
                max.y = std::max(max.y, mesh->mVertices[j].y);
                max.z = std::max(max.z, mesh->mVertices[j].z);
            }
        }
        std::cout << "min: " << min.x << ' ' << min.y << ' ' << min.z << " max: " << max.x << ' '
                  << max.y << ' ' << max.z << '\n';
        for (size_t i = 0; i < scene->mNumMaterials; ++i) {
            const auto *mat = scene->mMaterials[i];
            std::cout << "material " << i << ": " << mat->GetName().C_Str() << '\n';
        }
        for (size_t i = 0; i < scene->mNumTextures; ++i) {
            const auto *texture = scene->mTextures[i];
            std::cout << "texture " << i << ": " << texture->mFilename.C_Str() << '\n';
        }
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node
    // and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene) {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            // the node object only contains indices to index the actual objects in the scene.
            // the scene contains all the data, node is just to keep stuff organized (like relations
            // between nodes).
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the
        // children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    // checks all material textures of a given type and loads the textures if they're not loaded
    // yet. the required info is returned as a Texture struct.
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                              std::string typeName) {
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
};

#endif