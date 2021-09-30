#extension GL_ARB_explicit_uniform_location : enable
out vec4 FragColor;

//in vec3 fs_pos_in;
//in vec2 fs_texCoord_in;
//in vec3 fs_normal_in;

layout(location = 4) uniform sampler2D displacement;


void main()
{
    //    vec4 color = texture(displacement, fs_texCoord_in);
    FragColor = vec4(1, 0, 0, 0);
}