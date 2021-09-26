#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoord;
//out vec2 TexCoord;
//out vec3 Normal;
//out vec3 FragPos;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * transform * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_PointSize = 1;
//    FragPos = vec3(transform * model * vec4(aPos, 1.0));
}