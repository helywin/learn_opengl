#extension GL_ARB_explicit_uniform_location : enable
layout (location = 0) in vec3 aPos;

layout(location = 0) uniform mat4 transform;
layout(location = 1) uniform mat4 model;

out vec3 fs_pos_in;
//out vec2 fs_texCoord_in;
//out vec3 fs_normal_in;

void main()
{
    fs_pos_in = (transform * model * vec4(aPos.xyz, 1.0)).xyz;
//    fs_texCoord_in = aPos.xy;
//    fs_normal_in = vec3(0, 0, 1);
}