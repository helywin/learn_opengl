#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;

// real position in world axis
in vec3 FragPos;


struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_normals;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3  position;
    vec3  direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 4
uniform DirLight dirLight;
uniform Material material;
uniform vec3 viewPos;

vec4 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir, normal), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec4 ambientColor = texture(material.texture_diffuse, TexCoord);
    vec4 diffuseColor = texture(material.texture_diffuse, TexCoord);
    vec4 specularColor = texture(material.texture_diffuse, TexCoord);
    vec3 ambient = light.ambient * ambientColor.rgb * ambientColor.a;
    vec3 diffuse = light.diffuse * diff * diffuseColor.rgb;
    vec3 specular = light.specular * spec * specularColor.rgb * specularColor.a;

    return vec4(ambient + diffuse + specular, diffuseColor.a);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float distance = length(fragPos - light.position);
    float attenuation = 1.0 / (light.constant + distance * light.linear + (distance * distance) * light.quadratic);

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * texture(material.texture_diffuse, TexCoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse, TexCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular, TexCoord).rgb;

    return attenuation * (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(fragPos - light.position);
    float theta = dot(lightDir, light.direction);
    float epsilon = light.outerCutOff - light.cutOff;
    float intensity = clamp((light.cutOff - theta)/ epsilon, 0.0, 1.0);

    lightDir = -lightDir;
    float diff = max(dot(lightDir, normal), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * texture(material.texture_diffuse, TexCoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse, TexCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular, TexCoord).rgb;

    return intensity * (ambient + diffuse + specular);
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos + FragPos);
//    for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
//        result += calcPointLight(pointLights[i], norm, FragPos, viewDir);
//    }
//    result += calcSpotLight(spotLight, norm, FragPos, viewDir);
//    FragColor = vec4(result, 1.0);

//    vec4 color = texture(material.texture_diffuse, TexCoord);
//    if (color.a < 0.1) {
//        discard;
//    }
    FragColor = calcDirLight(dirLight, norm, viewDir);
}