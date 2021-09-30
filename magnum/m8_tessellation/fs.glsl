out vec4 color;

in vec3 fs_color;

void main(void)
{
    color = vec4(fs_color, 1.0);
}