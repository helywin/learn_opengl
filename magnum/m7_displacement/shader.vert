layout (location = 0) in vec3 aPos;
out vec4 textureColor;
out vec2 textureCoord;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D texture1;

void main()
{
    vec2 texCoord = vec2(aPos.x, aPos.y);
    float z = texture(texture1, texCoord).r;
    gl_Position = projection * view * transform * model * vec4(aPos.x, aPos.y, z * 10, 1.0);
    textureCoord = texCoord;
}