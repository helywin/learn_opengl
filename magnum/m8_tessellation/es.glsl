#extension GL_ARB_explicit_uniform_location : enable
layout (quads, equal_spacing, cw) in;

layout(location = 0) uniform sampler2D texture1;

out vec3 fs_color;

void main(void)
{
    float u = gl_TessCoord.x;
    float omu = 1 - u;// omu为1减去"u"
    float v = gl_TessCoord.y;
    float omv = 1 - v;// omv为1减去"v"

    //    color = gl_TessCoord; // color最后给到片段着色器时，值为(gl_TessCoord.x, gl_TessCoord.y, 0.0, 1.0)

    vec4 pos = omu * omv * gl_in[0].gl_Position +
    u * omv * gl_in[1].gl_Position +
    u * v * gl_in[2].gl_Position +
    omu * v * gl_in[3].gl_Position;

    gl_Position = pos;
    fs_color = vec3(texture(texture1, pos.xy + 0.5).rgb);
    //    gl_Position =
    //    (gl_TessCoord.x * gl_in[0].gl_Position) +
    //    (gl_TessCoord.y * gl_in[1].gl_Position) +
    //    (gl_TessCoord.z * gl_in[2].gl_Position);
}