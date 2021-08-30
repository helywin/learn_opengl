//
// Created by jiang on 2021/8/30.
//

#ifndef LEARN_OPENGL_TEXTURE2D_HPP
#define LEARN_OPENGL_TEXTURE2D_HPP

#include <string>

class Texture2D
{
    enum Type
    {
        rgb,
        rgba
    };
public:
    unsigned int ID;
    Texture2D(const std::string &file, Type type);
};


#endif //LEARN_OPENGL_TEXTURE2D_HPP
