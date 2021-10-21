#version 330 core
layout (points) in;

layout (triangle_strip, max_vertices = 3) out;

uniform mat4 view;
uniform mat4 projection;

void build_strips(vec4 position)
{
    gl_Position = projection * (view * position + vec4(-1, 0, 0, 0));
    EmitVertex();
    gl_Position = projection * (view * position + vec4(1, 0, 0, 0));
    EmitVertex();
    gl_Position = projection * (view * position + vec4(0, 1.732, 0, 0));
    EmitVertex();
    EndPrimitive();
}

void main()
{
    build_strips(gl_in[0].gl_Position);
}