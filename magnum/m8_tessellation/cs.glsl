layout (vertices = 4) out;

void main(void)
{
    if (gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = 20.0;
        gl_TessLevelInner[1] = 20.0;
        gl_TessLevelOuter[0] = 20.0;
        gl_TessLevelOuter[1] = 20.0;
        gl_TessLevelOuter[2] = 20.0;
        gl_TessLevelOuter[3] = 20.0;
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}