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
void init();

void draw();
void add();
void sub();
void clockWise();
void counterClockWise();
void forward();
void backward();
void left();
void right();
void rotateLeft();
void rotateRight();
void rotateUp();
void rotateDown();

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLuint VAO;
GLuint VBO;
GLuint EBO;
std::shared_ptr<Shader> shader;
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

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        add();
    } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        sub();
    } else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        clockWise();
    } else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        counterClockWise();
    } else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        forward();
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        backward();
    } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        left();
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        right();
    } else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        rotateLeft();
    } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        rotateRight();
    } else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        rotateUp();
    } else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        rotateDown();
    }
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

// 相机位置
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
// 目标位置
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
// 上向量
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float angle = 0;

void init()
{
    shader = std::make_shared<Shader>(
            ROOT_PATH PATH_SEP "src" PATH_SEP "6_camera" PATH_SEP "camera.vert",
            ROOT_PATH PATH_SEP "src" PATH_SEP "6_camera" PATH_SEP "camera.frag"
    );

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *) (3 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

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
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "transform"), 1, GL_FALSE,
                       glm::value_ptr(trans));

    glm::mat4 model = glm::mat4(1.0f);
//    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE,
                       glm::value_ptr(model));

    glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);

//    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT,
                                  0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));
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
    glUseProgram(shader->ID);
    glBindVertexArray(VAO);
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

float speed = 0.05;


void forward()
{
    auto direction = glm::normalize(cameraFront - cameraPos);
    cameraPos = cameraPos + speed * direction;
    glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
}

void backward()
{
    auto direction = glm::normalize(cameraFront - cameraPos);
    cameraPos = cameraPos - speed * direction;
    glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
}

void left()
{
    auto direction = glm::normalize(-glm::cross(cameraUp, cameraFront - cameraPos));
    cameraPos = cameraPos + speed * direction;
    cameraFront = cameraFront + speed * direction;
    glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
}

void right()
{
    auto direction = glm::normalize(-glm::cross(cameraUp, cameraFront - cameraPos));
    cameraPos = cameraPos - speed * direction;
    cameraFront = cameraFront - speed * direction;
    glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
}

void rotateLeft()
{
    auto direction = glm::normalize(-glm::cross(cameraUp, cameraFront - cameraPos));
//    cameraPos = cameraPos - speed * direction;
    cameraFront = cameraFront - speed * direction;
    glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
}

void rotateRight()
{
    auto direction = glm::normalize(-glm::cross(cameraUp, cameraFront - cameraPos));
    //    cameraPos = cameraPos - speed * direction;
    cameraFront = cameraFront + speed * direction;
    glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
}

void rotateUp()
{
    auto direction = glm::normalize(cameraUp);
    //    cameraPos = cameraPos - speed * direction;
    cameraFront = cameraFront + speed * direction;
    glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
}

void rotateDown()
{
    auto direction = glm::normalize(cameraUp);
    //    cameraPos = cameraPos - speed * direction;
    cameraFront = cameraFront - speed * direction;
    glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
}

bool posInit = false;
double lastXPos;
double lastYPos;

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (!posInit) {
        lastXPos = xpos;
        lastYPos = ypos;
        posInit = true;
        return;
    }
//    std::cout << xpos - lastXPos << ", " << ypos - lastYPos << std::endl;
    glm::vec3 direction = glm::vec3(xpos - lastXPos, -ypos + lastYPos, 0.0f);
    auto distance = glm::length(cameraFront - cameraPos);
    std::cout << "distance : " << distance << std::endl;
    cameraFront = cameraFront + 0.1f * distance * speed * direction;
    auto distance1 = cameraFront - cameraPos;
    distance1 = distance * glm::normalize(distance1);
    cameraFront = cameraPos + distance1;
    glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));

    lastXPos = xpos;
    lastYPos = ypos;
}