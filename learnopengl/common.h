#ifndef COMMON_H_
#define COMMON_H_

#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <learnopengl/shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// camera
extern Camera camera;
extern float lastX;
extern float lastY;
extern bool firstMouse;

// timing
extern float deltaTime;
extern float scale;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react
// accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    float sens = 6.0f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.ProcessMouseMovement(0.0f, sens);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.ProcessMouseMovement(0.0f, -sens);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camera.ProcessMouseMovement(sens, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) camera.ProcessMouseMovement(-sens, 0.0f);

    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) scale += deltaTime;
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) scale -= deltaTime;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        format == GL_RGBA
                            ? GL_CLAMP_TO_EDGE
                            : GL_REPEAT);  // for this tutorial: use GL_CLAMP_TO_EDGE to prevent
                                           // semi-transparent borders. Due to interpolation it
                                           // takes texels from next repeat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                        format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        throw std::runtime_error("Texture failed to load at path: "s + path);
        stbi_image_free(data);
    }

    return textureID;
}

#endif