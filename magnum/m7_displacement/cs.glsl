layout (vertices = 4) out;

in vec2 cs_texcoord[];
in vec3 cs_norm[];
out vec2 es_texcoord[];
out vec3 es_norm[];

void main(void)
{
    if (gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = 100.0;
        gl_TessLevelInner[1] = 100.0;
        gl_TessLevelOuter[0] = 100.0;
        gl_TessLevelOuter[1] = 100.0;
        gl_TessLevelOuter[2] = 100.0;
        gl_TessLevelOuter[3] = 100.0;
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    es_texcoord[gl_InvocationID] = cs_texcoord[gl_InvocationID];
    es_norm[gl_InvocationID] = cs_norm[gl_InvocationID];
}