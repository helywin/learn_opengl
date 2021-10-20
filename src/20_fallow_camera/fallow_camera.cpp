//
// Created by jiang on 2021/9/24.
//
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <stdio.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include <memory>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"
#include "Texture2D.hpp"
#include "Model.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void init();

void draw();
void add();
void sub();
void clockWise();
void counterClockWise();

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 800;
GLuint VAO;
GLuint lightVAO;
GLuint VBO;
GLuint EBO;
std::shared_ptr<Shader> shader;
std::shared_ptr<Shader> frameShader;
std::shared_ptr<Shader> lightShader;
std::shared_ptr<Shader> terrainShader;
std::shared_ptr<Model> modelCurve;
std::shared_ptr<Model> modelCube;
GLuint texture1;
GLuint texture2;
GLuint texture3;
float transparent = 0.2f;

// 位置, 纹理
float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
};

glm::vec3 cubePositions[10] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
};

glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)
};

// 位置向量
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
// 方向向量
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// 上方向
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float angle = 0;

ImVec4 clearColor(0.45f, 0.55f, 0.60f, 1.00f);
ImVec4 terrainColor(0.4f, 0.4f, 0.4f, 1.0f);
ImVec4 cubeColor(0.8f, 0.8f, 0.8f, 1.0f);
float ambientStrength = 0.1f;
float specularStrength = 0.5f;
int shininess = 5;

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char **)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
const char* glsl_version = "#version 100";
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
const char* glsl_version = "#version 150";
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
// GL 3.0 + GLSL 130
    const char *glsl_version = "#version 330 core";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

// Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1); // Enable vsync
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

// Setup Dear ImGui style
    ImGui::StyleColorsDark();
//ImGui::StyleColorsClassic();

// Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    init();

// Main loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Settings");
            ImGui::ColorEdit3("clear color", (float *) &clearColor);
            ImGui::ColorEdit3("cube color", (float *) &cubeColor);
            ImGui::ColorEdit3("terrain color", (float *) &terrainColor);
//            ImGui::SliderFloat("ambient", &ambientStrength, 0.0f, 1.0f);
//            ImGui::SliderFloat("specular", &specularStrength, 0.0f, 1.0f);
            ImGui::SliderInt((std::string("shininess: 2^") + std::to_string(shininess)).c_str(),
                             &shininess, 1, 8);
            ImGui::End();
        }

        // Rendering
        draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

// Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void init()
{
    shader = std::make_shared<Shader>(
            ROOT_PATH PATH_SEP "src" PATH_SEP "20_fallow_face" PATH_SEP "material.vert",
            ROOT_PATH PATH_SEP "src" PATH_SEP "20_fallow_face" PATH_SEP "material.frag"
    );

    frameShader = std::make_shared<Shader>(
            ROOT_PATH PATH_SEP "src" PATH_SEP "20_fallow_face" PATH_SEP "material.vert",
            ROOT_PATH PATH_SEP "src" PATH_SEP "20_fallow_face" PATH_SEP "singleColor.frag"
    );

    terrainShader = std::make_shared<Shader>(
            ROOT_PATH PATH_SEP "src" PATH_SEP "20_fallow_face" PATH_SEP "material.vert",
            ROOT_PATH PATH_SEP "src" PATH_SEP "20_fallow_face" PATH_SEP "terrain.frag"
    );

    modelCurve = std::make_shared<Model>(RES_DIR PATH_SEP "stencil/curve.obj");
    modelCube = std::make_shared<Model>(RES_DIR PATH_SEP "stencil/curve_cube.obj");

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *) (3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *) (6 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    //    glEnableVertexAttribArray(1);

    texture1 = loadTexture(RES_DIR PATH_SEP "container2.png");
    texture2 = loadTexture(RES_DIR PATH_SEP "container2_specular.png");


    // 设置不同的texture对应的采样器id
    shader->use();
    shader->setFloat("transparent", transparent);
    shader->setInt("material.diffuse", 0);
    shader->setInt("material.specular", 1);

    glm::mat4 trans = glm::mat4(1.0f);

    glm::mat4 model = glm::mat4(1.0f);
    //    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);

    //    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT,
                                  0.1f, 100.0f);

    shader->setMatrix4fv("transform", glm::value_ptr(trans));
    shader->setMatrix4fv("model", glm::value_ptr(model));
    shader->setMatrix4fv("view", glm::value_ptr(view));
    shader->setMatrix4fv("projection", glm::value_ptr(projection));

    frameShader->use();
    frameShader->setMatrix4fv("transform", glm::value_ptr(trans));
    frameShader->setMatrix4fv("model", glm::value_ptr(model));
    frameShader->setMatrix4fv("view", glm::value_ptr(view));
    frameShader->setMatrix4fv("projection", glm::value_ptr(projection));

    terrainShader->use();
    terrainShader->setMatrix4fv("transform", glm::value_ptr(trans));
    terrainShader->setMatrix4fv("model", glm::value_ptr(model));
    terrainShader->setMatrix4fv("view", glm::value_ptr(view));
    terrainShader->setMatrix4fv("projection", glm::value_ptr(projection));

}

void draw()
{
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glBindVertexArray(VAO);
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    terrainShader->use();
    terrainShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    terrainShader->setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
    terrainShader->setVec3("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
    terrainShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    terrainShader->setVec3("color", cubeColor.x, cubeColor.y, cubeColor.z);

    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    terrainShader->setMatrix4fv("view", glm::value_ptr(view));
    terrainShader->setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

//    glStencilFunc(GL_ALWAYS, 1, 0xFF);
//    glStencilMask(0xFF);

    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[0]);
        modelCube->draw(*shader);
    }


//    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
//    glStencilMask(0x00);
////    glDisable(GL_DEPTH_TEST);
//
//    frameShader->use();
//    frameShader->setMatrix4fv("view", glm::value_ptr(view));
//    frameShader->setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
//    {
//        float factor = 1.01;
//        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::scale(model, glm::vec3(factor, factor, factor));
//        frameShader->setMatrix4fv("model", glm::value_ptr(model));
//        modelCube->draw(*frameShader);
//    }
//    glStencilMask(0xFF);
//    glEnable(GL_DEPTH_TEST);
////     绘制相交线条
//    glClear(GL_STENCIL_BUFFER_BIT);
//    glStencilFunc(GL_ALWAYS, 1, 0xFF);
//    glStencilMask(0xFF);
//
//    terrainShader->use();
//    terrainShader->setVec3("color", terrainColor.x, terrainColor.y, terrainColor.z);
//    modelCurve->draw(*terrainShader);
//
//    glStencilFunc(GL_EQUAL, 1, 0xFF);
//    glStencilMask(0x00);
//    frameShader->use();
//    modelCube->draw(*frameShader);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);


}

float step = 0.02f;

void add()
{
    if (transparent < 1) {
        transparent += step;
    }
    glUniform1f(glGetUniformLocation(shader->ID, "transparent"), transparent);
}

void sub()
{
    if (transparent > 0) {
        transparent -= step;
    }
    glUniform1f(glGetUniformLocation(shader->ID, "transparent"), transparent);
}

void clockWise()
{
    angle += 10;
}

void counterClockWise()
{
    angle -= 10;
}

//float cameraSpeed = 0.05f;
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
bool showCursor = true;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
        showCursor = !showCursor;
        if (showCursor) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}

void processInput(GLFWwindow *window)
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    float cameraSpeed = deltaTime * 2.5f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        add();
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        sub();
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        clockWise();
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        counterClockWise();
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        cameraPos += glm::normalize(cameraUp) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        cameraPos -= glm::normalize(cameraUp) * cameraSpeed;
    }
}

bool posInit = false;
float lastX;
float lastY;
float yaw = -90.0f;
float pitch = 0;

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (showCursor) {
        posInit = false;
        return;
    }
    if (!posInit) {
        lastX = xpos;
        lastY = ypos;
        posInit = true;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    // 根据方位角计算方向向量
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

float fov = 45.0f;

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
    auto projection = glm::perspective(glm::radians(fov), (float) SCR_WIDTH / SCR_HEIGHT, 0.1f,
                                       100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));
}
