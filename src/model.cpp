#include <learnopengl/model.h>

void Model::Draw(Shader &shader) const {
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].Draw(shader);
    }
}

void Model::loadModel(std::string const &path, const std::vector<std::string> &mesh_names) {
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene *scene =
        importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                    aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        throw std::runtime_error("ERROR::ASSIMP");
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene, mesh_names);
}

void Model::processNode(aiNode *node, const aiScene *scene,
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

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;  // we declare a placeholder vector since assimp uses its own vector
                           // class that doesn't directly convert to glm's vec3 class so we
                           // transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0])  // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the
            // assumption that we won't use models where a vertex can have multiple texture
            // coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        } else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the
    // corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial *ai_material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be
    // named as 'texture_diffuseN' where N is a sequential number ranging from 1 to
    // MAX_SAMPLER_NUMBER. Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    std::multimap<std::string, Texture> textures;

    for (auto &[ai_type, type] : ai_texture_type_to_type) {
        std::vector<Texture> maps = loadMaterialTextures(ai_material, ai_type);
        for (auto &texture : maps) {
            textures.insert({type, texture});
        }
    }

    Material material;
    aiColor3D color(0.f, 0.f, 0.f);
    material.name = ai_material->GetName().C_Str();
    if (AI_SUCCESS != ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
        throw std::runtime_error("fail getting AI_MATKEY_COLOR_DIFFUSE");
    }
    material.color_diffuse.r = color.r;
    material.color_diffuse.g = color.g;
    material.color_diffuse.b = color.b;
    if (AI_SUCCESS != ai_material->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
        throw std::runtime_error("fail getting AI_MATKEY_COLOR_AMBIENT");
    }
    material.color_ambient.r = color.r;
    material.color_ambient.g = color.g;
    material.color_ambient.b = color.b;
    if (AI_SUCCESS != ai_material->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
        throw std::runtime_error("fail getting AI_MATKEY_COLOR_SPECULAR");
    }
    material.color_specular.r = color.r;
    material.color_specular.g = color.g;
    material.color_specular.b = color.b;
    if (AI_SUCCESS != ai_material->Get(AI_MATKEY_SHININESS, material.shininess)) {
        throw std::runtime_error("fail getting AI_MATKEY_SHININESS");
    }
    if (material.shininess <= 0) {
        material.shininess = 1;
    }
    if (AI_SUCCESS != ai_material->Get(AI_MATKEY_OPACITY, material.dissolve)) {
        throw std::runtime_error("fail getting AI_MATKEY_OPACITY");
    }
    assert(material.dissolve >= 0.0 && material.dissolve <= 1.0);

    // if (AI_SUCCESS != ai_material->Get(AI_MATKEY_TRANSPARENCYFACTOR, material.dissolve)) {
    //     throw std::runtime_error("fail getting AI_MATKEY_OPACITY");
    // }
    // if (AI_SUCCESS != ai_material->Get(AI_MATKEY_REFLECTIVITY, material.dissolve)) {
    //     throw std::runtime_error("fail getting AI_MATKEY_OPACITY");
    // }
    if (AI_SUCCESS != ai_material->Get(AI_MATKEY_REFRACTI, material.refracti)) {
        throw std::runtime_error("fail getting AI_MATKEY_OPACITY");
    }
    if (AI_SUCCESS != ai_material->Get(AI_MATKEY_COLOR_TRANSPARENT, color)) {
        throw std::runtime_error("fail getting AI_MATKEY_COLOR_TRANSPARENT");
    }
    // if (AI_SUCCESS != ai_material->Get(AI_MATKEY_COLOR_REFLECTIVE, color)) {
    //     throw std::runtime_error("fail getting AI_MATKEY_COLOR_TRANSPARENT");
    // }

    std::cout << "mesh: " << mesh->mName.C_Str() << ": verts: " << mesh->mNumVertices << '\n';
    glm::vec3 min(0), max(0);
    for (size_t j = 0; j < mesh->mNumVertices; ++j) {
        min.x = std::min(min.x, mesh->mVertices[j].x);
        min.y = std::min(min.y, mesh->mVertices[j].y);
        min.z = std::min(min.z, mesh->mVertices[j].z);

        max.x = std::max(max.x, mesh->mVertices[j].x);
        max.y = std::max(max.y, mesh->mVertices[j].y);
        max.z = std::max(max.z, mesh->mVertices[j].z);
    }
    std::cout << "min: " << min.x << ' ' << min.y << ' ' << min.z << " max: " << max.x << ' '
              << max.y << ' ' << max.z << '\n';

    std::cout << "material " << mesh->mMaterialIndex << ": " << ai_material->GetName().C_Str()
              << '\n';

    // return a mesh object created from the extracted mesh data
    return Mesh(mesh->mName.C_Str(), vertices, indices, textures, material);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type) {
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

unsigned int TextureFromFile(std::string_view filename, const std::string &directory) {
    auto path = (directory + '/').append(filename);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
        throw std::runtime_error("Texture failed to load");
    }

    return textureID;
}