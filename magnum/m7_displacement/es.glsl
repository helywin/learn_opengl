#extension GL_ARB_explicit_uniform_location : enable
layout (quads, equal_spacing, cw) in;

layout(location = 4) uniform sampler2D displacement;

in vec2 es_texcoord[];
in vec3 es_norm[];
out vec3 fs_color;

layout(location = 2) uniform mat4 view;
layout(location = 3) uniform mat4 projection;

void main(void)
{
    float u = gl_TessCoord.x;
    float omu = 1 - u;// omu为1减去"u"
    float v = gl_TessCoord.y;
    float omv = 1 - v;// omv为1减去"v"

    vec4 pos = omu * omv * gl_in[0].gl_Position +
               u * omv * gl_in[1].gl_Position +
               u * v * gl_in[2].gl_Position +
               omu * v * gl_in[3].gl_Position;

    vec2 tex = omu * omv * es_texcoord[0] +
               u * omv * es_texcoord[1]  +
               u * v * es_texcoord[2]  +
               omu * v * es_texcoord[3] ;

    vec4 color = texture(displacement, tex);
    gl_Position = projection * view * (pos + vec4(es_norm[0], 0) * color.r / 10);
    fs_color = color.rgb;

}