#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_tessellation_shader : enable
layout(triangles, equal_spacing, ccw) in;

in vec2 es_pos_in[];

//out vec2 fs_pos_in;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
//    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
    return (v0 + v1 + v2) / 3;
}

void main()
{

    vec2 pos = interpolate2D(es_pos_in[0], es_pos_in[1], es_pos_in[2]);
    gl_Position = vec4(pos, 0.0, 1.0);
}