#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>  // holds all OpenGL type declarations

#include <string>
#include <string_view>
#include <vector>

struct Texture {
    unsigned int id;
    unsigned int num_components;
    std::string type;
    std::string path;
};

// utility function for loading a 2D texture from file
// ---------------------------------------------------
std::pair<unsigned int, unsigned int> loadTexturePair(const std::string& path,
                                                      bool gammaCorrection = false);
unsigned int loadTexture(const std::string& path, bool gammaCorrection = false);

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces);

Texture TextureFromFile(std::string_view filename, const std::string& directory);

#endif