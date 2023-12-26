#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <learnopengl/common.h>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 5.0f, 3.0f));

float scale = 1.0;
bool enable = false;
bool enable_flashlight = true;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    // stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader lightingShader("src/3.model_loading/1.model_loading/1.model_loading.vs",
                          "src/3.model_loading/1.model_loading/1.model_loading.fs");

    Shader lightCubeShader("src/3.model_loading/1.model_loading/6.light_cube.vs",
                           "src/3.model_loading/1.model_loading/6.light_cube.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 0.0f,
        0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  1.0f,
        0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,
        0.0f,  -1.0f, 0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
        0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,
        0.0f,  1.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, -1.0f,
        0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f,
        0.0f,  0.0f,  0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 1.0f,
        0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,
        0.0f,  0.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 0.0f,
        -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f,  0.0f,
        0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,
        -1.0f, 0.0f,  0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
        1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,
        1.0f,  0.0f,  0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // load models
    // -----------
    std::vector<RenderModel> models;
    models.push_back({Model("resources/objects/cottage/cottage_obj.obj"), glm::vec3{0, 0, -10},
                      glm::vec3{0.4f}, 0});
    models.push_back({Model("resources/objects/cottage2/Cottage_FREE.obj"), glm::vec3{0, 0, 10},
                      glm::vec3{1.2}, 0});
    models.push_back({Model("resources/objects/tower/wooden_watch_tower2.obj"), glm::vec3{10, 0, 0},
                      glm::vec3{1}, 0});
    models.push_back(
        {Model("resources/objects/nanosuit/nanosuit.obj"), glm::vec3{0, 0, 0}, glm::vec3{0.18}, 0});

    // lighting
    std::vector<glm::vec3> pointLightPositions{
        glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(2.0f, 1.0f, -4.0f), glm::vec3(4.0f, 2.0f, 4.0f),
        glm::vec3(0.0f, 3.0f, -3.0f)};

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window, &scale, &enable, &enable_flashlight);

        // draw in wireframe
        glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // glEnable(GL_CULL_FACE);

        // don't forget to enable shader before setting uniforms
        lightingShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        lightingShader.setBool("spotLight.enabled", enable_flashlight);
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);

        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        lightingShader.setVec3("spotLight.ambient", ambientColor);
        lightingShader.setVec3("spotLight.diffuse", diffuseColor);
        lightingShader.setVec3("spotLight.specular", lightColor);

        lightingShader.setVec3("viewPos", camera.Position);

        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", glm::vec3(0.1));
        lightingShader.setVec3("dirLight.diffuse", glm::vec3(0.2));
        lightingShader.setVec3("dirLight.specular", glm::vec3(0.8));

        std::vector<glm::vec3> lightColors(pointLightPositions.size());
        lightColors.at(0) = glm::vec3{1.0, 1.0, 0.0};
        lightColors.at(1) = glm::vec3{1.0, 0.0, 0.0};
        lightColors.at(2) = glm::vec3{0.0, 0.0, 1.0};
        lightColors.at(3) = glm::vec3{1.0};

        for (size_t i = 0; i < lightColors.size(); ++i) {
            // lightColors[i].x = sin(glfwGetTime() * (1) * 0.2 + 20 * i);
            // lightColors[i].y = cos(glfwGetTime() * (1) + 45 * i);
            // lightColors[i].z = sin(glfwGetTime() * (1) * 0.5 + 30 * i);

            glm::vec3 diffuseColor = lightColors[i] * glm::vec3(0.5f);
            glm::vec3 ambientColor = lightColors[i] * glm::vec3(0.1f);

            pointLightPositions[i].x = std::sin((float)glfwGetTime() / 2 + 45 * i) * 5;
            // pointLightPositions[i].y = (1 + std::cos((float)glfwGetTime() / (1))) * (i + 5);
            // pointLightPositions[i].z = std::cos((float)glfwGetTime() * (1) + 90 * i) * (i + 15);

            std::string name = "pointLights[";
            name.append(std::to_string(i)).append("].");

            lightingShader.setVec3(name + "position", pointLightPositions[i]);

            lightingShader.setFloat(name + "constant", 1.0f);
            lightingShader.setFloat(name + "linear", 0.09f);
            lightingShader.setFloat(name + "quadratic", 0.032f);

            lightingShader.setVec3(name + "ambient", ambientColor);
            lightingShader.setVec3(name + "diffuse", diffuseColor);
            lightingShader.setVec3(name + "specular", lightColors[i]);
        }

        // render the loaded model
        for (const auto& mod : models) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, mod.pos);
            model = glm::scale(model, mod.scale);
            model = glm::rotate(model, glm::radians(mod.angle), glm::vec3(0.0f, 1.0f, 0.0f));
            lightingShader.setMat4("model", model);
            mod.model.Draw(lightingShader);
        }

        // also draw the lamp object
        lightCubeShader.use();
        for (size_t i = 0; i < pointLightPositions.size(); ++i) {
            lightCubeShader.setVec3("lightColor", lightColors[i]);
            lightCubeShader.setMat4("projection", projection);
            lightCubeShader.setMat4("view", view);
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));  // a smaller cube
            lightCubeShader.setMat4("model", model);

            glBindVertexArray(lightCubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}