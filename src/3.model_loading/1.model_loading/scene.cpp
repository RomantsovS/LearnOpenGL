#include "scene.h"

void Scene::AddModel(const std::string& file_name, glm::vec3 pos, glm::vec3 scale, float angle,
                     const std::vector<std::string>& mesh_names) {
    if (models.count(file_name) == 0) {
        models.emplace(std::make_pair(file_name, Model(file_name, mesh_names)));
    }

    for (const auto& mesh : models.at(file_name).meshes) {
        if (mesh.isTransparent()) {
            render_meshes_transparent.push_back(RenderMesh{&mesh, pos, scale, angle});
        } else {
            render_meshes.push_back(RenderMesh{&mesh, pos, scale, angle});
        }
    }
}

void Scene::Render(Shader& shader) {
    for (const auto& mesh : render_meshes) {
        DrawMesh(shader, mesh);
    }
}

void Scene::RenderTransparent(Shader& shader) {
    for (const auto& mesh : render_meshes_transparent) {
        DrawMesh(shader, mesh);
    }
}

void Scene::DrawMesh(Shader& shader, const RenderMesh& mesh) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, mesh.pos);
    model = glm::scale(model, mesh.scale);
    model = glm::rotate(model, glm::radians(mesh.angle), glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("model", model);
    mesh.Draw(shader);
}