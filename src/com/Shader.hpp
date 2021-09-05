//
// Created by jiang on 2021/8/30.
//

#ifndef LEARN_OPENGL_SHADER_HPP
#define LEARN_OPENGL_SHADER_HPP


#include <glad/glad.h> // 包含glad来获取所有的必须OpenGL头文件
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>


class Shader
{
public:
    // 程序ID
    unsigned int ID;

    // 构造器读取并构建着色器
    Shader(const std::string &vertexPath, const std::string &fragmentPath);
    // 使用/激活程序
    void use() const;
    // uniform工具函数
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &v) const;
    void setVec4(const std::string &name, const glm::vec4 &v) const;
    void setVec3(const std::string &name, float v1, float v2, float v3) const;
    void setVec4(const std::string &name, float v1, float v2, float v3, float v4) const;
    void setMatrix4fv(const std::string &name, float *v) const;
};

#endif //LEARN_OPENGL_SHADER_HPP
