layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec4 textureColor;
out vec2 textureCoord;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D texture1;

void main()
{
    float z = texture(texture1, aTexCoord).r;
    gl_Position = projection * view * transform * model * vec4(aPos.x, aPos.y, z * 10, 1.0);
    textureCoord = aTexCoord;
}