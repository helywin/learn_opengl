layout(vertices = 3) out;

in vec3 cs_pos_in[];
//in vec2 cs_texCoord_in[];
//in vec3 cs_normal_in[];

out vec3 es_pos_in[];
//out vec2 es_texCoord_in[];
//out vec3 es_normal_in[];

void main()
{
    gl_TessLevelOuter[0] = 4.0;
    gl_TessLevelOuter[1] = 4.0;
    gl_TessLevelOuter[2] = 4.0;

    gl_TessLevelInner[0] = 4.0;

    es_pos_in[gl_InvocationID] = cs_pos_in[gl_InvocationID];
//    es_texCoord_in[gl_InvocationID] = cs_texCoord_in[gl_InvocationID];
//    es_normal_in[gl_InvocationID] = cs_normal_in[gl_InvocationID];
}