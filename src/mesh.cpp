#include <assimp/scene.h>
#include <learnopengl/mesh.h>

std::map<aiTextureType, std::string> ai_texture_type_to_type = {
    {aiTextureType_DIFFUSE, "texture_diffuse"},
    {aiTextureType_SPECULAR, "texture_specular"},
    {aiTextureType_AMBIENT, "texture_reflection"}};
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
}

void Mesh::Draw(Shader &shader) const {
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    unsigned int reflectionNr = 1;
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
        else if (name == "texture_reflection")
            number = std::to_string(reflectionNr++);  // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        shader.setInt(("material." + name + number).c_str(), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, id);

        ++i;
    };

    for (const auto &dummy_texture : Mesh::dummy_textures) {
        auto range = textures.equal_range(dummy_texture.first);
        for (auto iter = range.first; iter != range.second; ++iter) {
            bind_texture(iter->second.type, iter->second.id);
        }
        if (range.first == range.second) {
            bind_texture(dummy_texture.second.type, dummy_texture.second.id);
        }
    }

    if (material.color_ambient == glm::vec3(0.0) && material.color_diffuse == glm::vec3(0.0)) {
        shader.setVec3("material.color_ambient", glm::vec3(1.0));
        shader.setVec3("material.color_diffuse", glm::vec3(1.0));
    } else {
        shader.setVec3("material.color_ambient", material.color_ambient);
        shader.setVec3("material.color_diffuse", material.color_diffuse);
    }
    shader.setVec3("material.color_specular", material.color_specular);
    shader.setFloat("material.shininess", material.shininess);
    shader.setFloat("material.dissolve", material.dissolve);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::loadDummyTextures() {
    std::unordered_map<aiTextureType, std::string> dummy_textures{
        {aiTextureType_DIFFUSE, "dummy_white.png"},
        {aiTextureType_SPECULAR, "dummy_white.png"},
        {aiTextureType_AMBIENT, "dummy_black.png"}};
    for (auto &[ai_type, name] : dummy_textures) {
        Texture texture;
        texture.id = TextureFromFile(name, "resources/textures");
        texture.type = ai_texture_type_to_type[ai_type];
        texture.path = name;
        Mesh::dummy_textures[texture.type] = texture;
    }
}

void Mesh::setupMesh() {
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly
    // to a glm::vec3/2 array which again translates to 3/2 floats which translates to a byte
    // array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0],
                 GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}