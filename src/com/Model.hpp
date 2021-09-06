//
// Created by jiang on 2021/9/6.
//

#ifndef LEARN_OPENGL_MODEL_HPP
#define LEARN_OPENGL_MODEL_HPP

#include <string>

class ModelPrivate;
class Shader;

class Model
{
public:
    explicit Model(const std::string &path);
    ~Model();
    void draw(const Shader &shader) const;
private:
    ModelPrivate *d;
};


#endif //LEARN_OPENGL_MODEL_HPP
