#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * transform * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    FragPos = vec3(transform * model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(transform * model))) * aNormal;
}