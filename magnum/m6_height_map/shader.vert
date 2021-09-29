layout (location = 0) in vec3 aPos;
out vec4 textureColor;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * transform * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    textureColor = vec4(aPos.z, 0, 0, 1.0);
}