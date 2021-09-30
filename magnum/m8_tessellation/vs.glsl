#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_tessellation_shader : enable
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec2 fs_pos_in;

void main()
{
    fs_pos_in = aPos;
}