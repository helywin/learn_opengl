out vec4 FragColor;
in vec4 textureColor;
in vec2 textureCoord;
uniform sampler2D texture1;


void main()
{
    vec4 color = texture(texture1, textureCoord);
    FragColor = color;
}