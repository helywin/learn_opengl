//
// Created by jiang on 2021/8/30.
//

#include <iostream>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void init();

void draw();
void add();
void sub();
void clockWise();
void counterClockWise();

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLuint VAO;
GLuint lightVAO;
GLuint VBO;
GLuint EBO;
std::shared_ptr<Shader> shader;
std::shared_ptr<Shader> lightShader;
GLuint texture1;
GLuint texture2;
float transparent = 0.2f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // 注册鼠标消息
    glfwSetCursorPosCallback(window, mouse_callback);
    // 隐藏鼠标显示
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // 先初始化glad才能调用OpenGL函数
    init();
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 位置, 纹理
float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
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

// 位置向量
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
// 方向向量
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// 上方向
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float angle = 0;

void init()
{
    shader = std::make_shared<Shader>(
            ROOT_PATH PATH_SEP "src" PATH_SEP "7_light" PATH_SEP "light.vert",
            ROOT_PATH PATH_SEP "src" PATH_SEP "7_light" PATH_SEP "light.frag"
    );

    lightShader = std::make_shared<Shader>(
            ROOT_PATH PATH_SEP "src" PATH_SEP "7_light" PATH_SEP "light_source.vert",
            ROOT_PATH PATH_SEP "src" PATH_SEP "7_light" PATH_SEP "light_source.frag"
    );

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
//                          (void *) (3 * sizeof(float)));

    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);

    // 加载时上下翻转图片
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char *data = stbi_load(RES_DIR
                                    PATH_SEP "container.jpg", &width, &height, &nrChannels,
                                    0);
    if (!data) {
        std::cout << "load image failed!" << std::endl;
        exit(-1);
    }

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);


    data = stbi_load(RES_DIR
                     PATH_SEP "awesomeface.png", &width, &height, &nrChannels, 0);
    if (!data) {
        std::cout << "load image failed!" << std::endl;
        exit(-1);
    }

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // 设置不同的texture对应的采样器id
    shader->use();
    glUniform1i(glGetUniformLocation(shader->ID, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shader->ID, "texture2"), 1);
    glUniform1f(glGetUniformLocation(shader->ID, "transparent"), transparent);

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

    lightShader->use();
    lightShader->setMatrix4fv("transform", glm::value_ptr(trans));
    lightShader->setMatrix4fv("model", glm::value_ptr(model));
    lightShader->setMatrix4fv("view", glm::value_ptr(view));
    lightShader->setMatrix4fv("projection", glm::value_ptr(projection));
    glEnable(GL_DEPTH_TEST);
}

void draw()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glBindVertexArray(VAO);
    shader->use();
    shader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 model = glm::mat4(1.0f);
    shader->setMatrix4fv("model", glm::value_ptr(model));
    shader->setMatrix4fv("view", glm::value_ptr(view));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(lightVAO);
    lightShader->use();
    lightShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    lightShader->setMatrix4fv("model", glm::value_ptr(model));
    lightShader->setMatrix4fv("view", glm::value_ptr(view));
    glDrawArrays(GL_TRIANGLES, 0, 36);
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
    std::cout << "angle: " << angle << std::endl;
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "transform"), 1, GL_FALSE,
                       glm::value_ptr(trans));
}

void counterClockWise()
{
    angle -= 10;
    std::cout << "angle: " << angle << std::endl;
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "transform"), 1, GL_FALSE,
                       glm::value_ptr(trans));
}

//float cameraSpeed = 0.05f;
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

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
}

bool posInit = false;
float lastX;
float lastY;
float yaw = -90.0f;
float pitch = 0;

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{

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