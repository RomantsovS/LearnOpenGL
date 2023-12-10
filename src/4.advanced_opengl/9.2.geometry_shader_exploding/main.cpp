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
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
std::vector<glm::vec3> pointLightPositions{
    glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f), glm::vec3(-4.0f, 2.0f, -12.0f),
    glm::vec3(0.0f, 0.0f, -3.0f)};

float scale = 1.0;
bool enable = true;

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
    Shader lightingShader(
        "src/4.advanced_opengl/9.2.geometry_shader_exploding/9.2.geometry_shader.vs",
        "src/4.advanced_opengl/9.2.geometry_shader_exploding/9.2.geometry_shader.fs",
        "src/4.advanced_opengl/9.2.geometry_shader_exploding/9.2.geometry_shader.gs");

    // load models
    // -----------
    std::vector<RenderModel> models;
    models.push_back({Model("resources/objects/nanosuit/nanosuit.obj"), glm::vec3{0, 0, 0},
                      glm::vec3{0.5f, 0.5f, 0.5f}, 0});
    // models.push_back({Model("resources/objects/cottage2/Cottage_FREE.obj"), glm::vec3{0, 0, 10},
    //                   glm::vec3{1, 1, 1}, 0});
    // models.push_back({Model("resources/objects/tower/wooden_watch_tower2.obj"), glm::vec3{10, 0,
    // 0},
    //                   glm::vec3{1, 1, 1}, 0});

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // draw in wireframe
        glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        lightingShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // add time component to geometry shader in the form of a uniform
        lightingShader.setFloat("time", static_cast<float>(glfwGetTime()));

        // render the loaded model
        for (const auto& mod : models) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, mod.pos);
            model = glm::scale(model, mod.scale);
            model = glm::rotate(model, glm::radians(mod.angle), glm::vec3(0.0f, 1.0f, 0.0f));
            lightingShader.setMat4("model", model);
            mod.model.Draw(lightingShader);
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