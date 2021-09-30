#extension GL_ARB_explicit_uniform_location : enable
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

//out vec3 fs_pos_in;
//out vec2 fs_texCoord_in;
//out vec3 fs_normal_in;

void main()
{
    vec4 pos = vec4(aPos.xy, 0.0, 1.0);
    //    fs_pos_in = pos.xyz;
    //    fs_texCoord_in = aPos.xy;
    //    fs_normal_in = vec3(0, 0, 1);
    gl_Position = pos;
}