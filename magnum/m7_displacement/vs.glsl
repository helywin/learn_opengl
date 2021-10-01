#extension GL_ARB_explicit_uniform_location : enable
//layout (location = 0) in vec4 aPos;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 transform;

out vec2 cs_texcoord;
out vec3 cs_norm;

void main(void)
{
    const vec4 vertices[] = vec4[](vec4( 0.5, -0.5, 0.0, 1.0),
                                   vec4(-0.5, -0.5, 0.0, 1.0),
                                   vec4(-0.5,  0.5, 0.0, 1.0),
                                   vec4( 0.5,  0.5, 0.0, 1.0));
    gl_Position = transform * model * vertices[gl_VertexID];
    cs_texcoord = vertices[gl_VertexID].xy + 0.5;
    cs_norm = (transform * model * vec4(0, 0, 1, 1)).xyz;
//    gl_Position = aPos;
}