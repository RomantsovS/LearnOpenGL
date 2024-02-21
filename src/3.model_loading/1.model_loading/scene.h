#ifndef SCENE_H
#define SCENE_H

#include <string_view>
#include <unordered_map>

#include "learnopengl/model.h"

struct RenderMesh {
    const Mesh *mesh = nullptr;
    glm::vec3 pos;
    glm::vec3 scale;
    float angle;

    void Draw(Shader &shader) const { mesh->Draw(shader); }
};

class Scene {
   public:
    void AddModel(const std::string &file_name, glm::vec3 pos, glm::vec3 scale, float angle,
                  const std::vector<std::string> &mesh_names = {});

    void Render(Shader &shader);

   private:
    void DrawMesh(Shader &shader, const RenderMesh &mesh);

    std::unordered_map<std::string, Model> models;
    std::vector<RenderMesh> render_meshes;
    std::vector<RenderMesh> render_meshes_dissolve;
};

#endif