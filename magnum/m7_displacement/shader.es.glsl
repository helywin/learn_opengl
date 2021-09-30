#extension GL_ARB_explicit_uniform_location : enable
layout(triangles, equal_spacing, ccw) in;

layout(location = 2) uniform mat4 view;
layout(location = 3) uniform mat4 projection;
//uniform sampler2D texture1;

in vec3 es_pos_in[];
//in vec2 es_texCoord_in[];
//in vec3 es_normal_in[];

out vec3 fs_pos_in;
//out vec2 fs_texCoord_in;
//out vec3 fs_normal_in;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
    // Interpolate the attributes of the output vertex using the barycentric coordinates
//    fs_texCoord_in = interpolate2D(es_texCoord_in[0], es_texCoord_in[1], es_texCoord_in[2]);
//    fs_normal_in = interpolate3D(es_normal_in[0], es_normal_in[1], es_normal_in[2]);
//    fs_normal_in = normalize(fs_normal_in);
    fs_pos_in = interpolate3D(es_pos_in[0], es_pos_in[1], es_pos_in[2]);

    // Displace the vertex along the normal
//    float Displacement = texture(texture1, fs_texCoord_in.xy).x;
//    fs_pos_in += fs_normal_in * Displacement * 1.0;
    gl_Position = projection * view * vec4(fs_pos_in, 1.0);
}