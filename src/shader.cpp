#include <learnopengl/shader.h>

Shader::Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile(vertexPath);
    std::ifstream fShaderFile(fragmentPath);
    std::ifstream gShaderFile(geometryPath);
    // ensure ifstream objects can throw exceptions:
    // vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    // fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        // vShaderFile.open(vertexPath);
        if (!vShaderFile.is_open())
            throw std::runtime_error(std::string("failed to open: ") + vertexPath);
        // fShaderFile.open(fragmentPath);
        if (!fShaderFile.is_open())
            throw std::runtime_error(std::string("failed to open: ") + fragmentPath);
        if (geometryPath && !gShaderFile.is_open())
            throw std::runtime_error(std::string("failed to open: ") + geometryPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if (geometryPath != nullptr) {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    } catch (std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        throw;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    if (!checkCompileErrors(vertex, "VERTEX")) {
        throw std::runtime_error("vertex shader compile error");
    };
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    if (!checkCompileErrors(fragment, "FRAGMENT")) {
        std::runtime_error("fragment shader compile error");
    }
    // if geometry shader is given, compile geometry shader
    unsigned int geometry;
    if (geometryPath != nullptr) {
        const char *gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometryPath != nullptr) glAttachShader(ID, geometry);
    glLinkProgram(ID);
    if (!checkCompileErrors(ID, "PROGRAM")) {
        throw std::runtime_error("shader link error");
    }
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr) glDeleteShader(geometry);
}
// activate the shader
// ------------------------------------------------------------------------
void Shader::use() const { glUseProgram(ID); }
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value) {
    glUniform1i(getUniformLocation(name), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) {
    glUniform1i(getUniformLocation(name), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) {
    glUniform1f(getUniformLocation(name), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) {
    glUniform2fv(getUniformLocation(name), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) {
    glUniform2f(getUniformLocation(name), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) {
    glUniform3fv(getUniformLocation(name), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) {
    glUniform3f(getUniformLocation(name), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) {
    glUniform4fv(getUniformLocation(name), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) {
    glUniform4f(getUniformLocation(name), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) {
    glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

GLint Shader::getUniformLocation(const std::string &name) {
    auto ret = glGetUniformLocation(ID, name.c_str());
    // if (ret < 0) {
    //     GLenum err;
    //     while ((err = glGetError()) != GL_NO_ERROR) {
    //         std::cerr << "err:" << err << '\n';
    //     }
    //     throw std::runtime_error("invalid uniform: " + name);
    // }
    return ret;
}

bool Shader::checkCompileErrors(unsigned int shader, std::string type) {
    GLint success{};
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    }
    return success;
}