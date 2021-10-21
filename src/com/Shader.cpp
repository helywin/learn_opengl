//
// Created by jiang on 2021/8/30.
//

#include "Shader.hpp"

std::string readFile(const std::string &fileName)
{
    std::ifstream is;
    is.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        is.open(fileName);
        std::stringstream stream;
        stream << is.rdbuf();
        is.close();
        return stream.str();
    } catch (std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n" << e.what() << std::endl;
        return "";
    }
}

unsigned int createAndCompileShader(const std::string &shaderCode, Shader::ShaderType shaderType)
{
    int success;
    char infoLog[512];
    const char *code = shaderCode.c_str();
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    };
    return shader;
}

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath)
{
    // 1. 从文件路径中获取顶点/片段着色器
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);


    // 2. 编译着色器
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // 顶点着色器
    vertex = createAndCompileShader(vertexCode, st_vertex);

    // 片段着色器也类似
    fragment = createAndCompileShader(fragmentCode, st_fragment);

    // 着色器程序
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // 打印连接错误（如果有的话）
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(const std::string &vertexPath, const std::string &geometryPath,
               const std::string &fragmentPath)
{
    // 1. 从文件路径中获取顶点/片段着色器
    std::string vertexCode = readFile(vertexPath);
    std::string geometryCode = readFile(geometryPath);
    std::string fragmentCode = readFile(fragmentPath);


    // 2. 编译着色器
    unsigned int vertex, geometry, fragment;
    int success;
    char infoLog[512];

    // 顶点着色器
    vertex = createAndCompileShader(vertexCode, st_vertex);

    // 几何着色器
    geometry = createAndCompileShader(geometryCode, st_geometry);

    // 片段着色器也类似
    fragment = createAndCompileShader(fragmentCode, st_fragment);

    // 着色器程序
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, geometry);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // 打印连接错误（如果有的话）
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
    glDeleteShader(vertex);
    glDeleteShader(geometry);
    glDeleteShader(fragment);
}

void Shader::use() const
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &v) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), v.x, v.y, v.z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &v) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), v.x, v.y, v.z, v.w);
}

void Shader::setVec3(const std::string &name, float v1, float v2, float v3) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), v1, v2, v3);
}

void Shader::setVec4(const std::string &name, float v1, float v2, float v3, float v4) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), v1, v2, v3, v4);
}

void Shader::setMatrix4fv(const std::string &name, float *v) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, v);
}
