#include <learnopengl/model.h>

std::map<aiTextureType, std::string> ai_texture_type_to_type = {
    {aiTextureType_DIFFUSE, "texture_diffuse"}, {aiTextureType_SPECULAR, "texture_specular"}};
std::map<std::string, Texture> Mesh::dummy_textures;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::multimap<std::string, Texture> textures, Material material) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->material = material;

    // now that we have all the required data, set the vertex buffers and its attribute
    // pointers.
    setupMesh();

    if (Mesh::dummy_textures.empty()) {
        loadDummyTextures();
    }
}

void Mesh::Draw(Shader &shader) const {
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    size_t i = 0;

    auto bind_texture = [&](const std::string &type, unsigned id) {
        glActiveTexture(GL_TEXTURE0 + i);  // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);  // transfer unsigned int to string
        else if (name == "texture_normal")
            number = std::to_string(normalNr++);  // transfer unsigned int to string
        else if (name == "texture_height")
            number = std::to_string(heightNr++);  // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        shader.setInt(("material." + name + number).c_str(), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, id);
    };

    for (const auto &dummy_texture : Mesh::dummy_textures) {
        auto range = textures.equal_range(dummy_texture.first);
        for (auto iter = range.first; iter != range.second; ++iter) {
            bind_texture(iter->second.type, iter->second.id);
            ++i;
        }
        if (range.first == range.second) {
            bind_texture(dummy_texture.second.type, dummy_texture.second.id);
        }
    }

    shader.setFloat("material.shininess", material.shininess);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::loadDummyTextures() {
    for (auto &[ai_type, type] : ai_texture_type_to_type) {
        Texture texture;
        texture.id = TextureFromFile("dummy.png", "resources/textures");
        texture.type = type;
        texture.path = "dummy.png";
        Mesh::dummy_textures[type] = texture;
    }
}