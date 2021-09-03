#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;

// real position in world axis
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float transparent;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};


struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;
uniform sampler2D matrix;

uniform vec3 viewPos;

void main()
{
    // ambient
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;

    // reflect
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * texture(material.specular, TexCoord).rgb * spec;

    vec3 matrixColor = texture(matrix, TexCoord).rgb;
    float green = length(matrixColor);

    FragColor = vec4(attenuation * (ambient + diffuse + specular), transparent);
}