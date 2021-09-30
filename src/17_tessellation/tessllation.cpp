#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void init();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
GLuint VAO;
GLuint VBO;
GLuint EBO;
GLint shaderProgram;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
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
    // 先初始化glad才能调用OpenGL函数
    init();
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_PATCHES, 0, 3);
        glBindVertexArray(0);
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
    }
}

void init()
{
    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f
    };

    static const char *vertexShaderSource = R"(
#version 420 core
#extension GL_ARB_tessellation_shader : enable
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec2 fs_pos_in;

void main()
{
    fs_pos_in = aPos;
}
)";

    static const char *tessellationControlSource = R"(
#version 420 core
#extension GL_ARB_tessellation_shader : enable
layout(vertices = 3) out;

in vec2 cs_pos_in[];
//in vec2 cs_texCoord_in[];
//in vec3 cs_normal_in[];

out vec2 es_pos_in[];
//out vec2 es_texCoord_in[];
//out vec3 es_normal_in[];

void main()
{
    gl_TessLevelOuter[0] = 3.0;
    gl_TessLevelOuter[1] = 3.0;
    gl_TessLevelOuter[2] = 3.0;

    gl_TessLevelInner[0] = gl_TessLevelOuter[2];

    es_pos_in[gl_InvocationID] = cs_pos_in[gl_InvocationID];
    //    es_texCoord_in[gl_InvocationID] = cs_texCoord_in[gl_InvocationID];
    //    es_normal_in[gl_InvocationID] = cs_normal_in[gl_InvocationID];
}
)";
    static const char *tessellationEvalSource = R"(
#version 420 core
#extension GL_ARB_tessellation_shader : enable
layout(triangles, equal_spacing, ccw) in;

in vec2 es_pos_in[];

out vec2 gs_pos_in;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
//    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
    return (v0 + v1 + v2) / 3;
}

void main()
{

    vec2 pos = interpolate2D(es_pos_in[0], es_pos_in[1], es_pos_in[2]);
    gl_Position = vec4(pos, 0.0, 1.0);
}
)";

    static const char *geometrySource = R"(
#version 420 core
#extension GL_ARB_tessellation_shader : enable
layout(triangles) in;
layout (triangle_strip, max_vertices = 16)out;
in vec2 gs_pos_in[];

void main()
{
    gl_Position = vec4(gl_in[0].gl_Position.xy, 0, 1);
    EmitVertex();

    gl_Position = vec4(gl_in[1].gl_Position.xy, 0, 1);
    EmitVertex();

    gl_Position = vec4(gl_in[2].gl_Position.xy, 0, 1);
    EmitVertex();
}
)";

    static const char *fragmentShaderSource = R"(
#version 420 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_tessellation_shader : enable
out vec4 FragColor;

void main()
{
    FragColor = vec4(1, 0, 0, 0);
}
)";

    auto getError = [](unsigned int shader) {
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            return;
        }
    };

    GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    getError(vertexShader);

    GLint tessellationControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tessellationControlShader, 1, &tessellationControlSource, nullptr);
    glCompileShader(tessellationControlShader);
    getError(tessellationControlShader);

    GLint tessellationEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(tessellationEvalShader, 1, &tessellationEvalSource, nullptr);
    glCompileShader(tessellationEvalShader);
    getError(tessellationEvalShader);

    GLint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &geometrySource, nullptr);
    glCompileShader(geometryShader);
    getError(geometryShader);

    GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    getError(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, tessellationControlShader);
    glAttachShader(shaderProgram, tessellationEvalShader);
    glAttachShader(shaderProgram, geometryShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(tessellationControlShader);
    glDeleteShader(tessellationEvalShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glPatchParameteri(GL_TRIANGLES, 3);
}