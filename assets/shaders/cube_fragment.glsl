#version 460 core
out vec4 FragColor;

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

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform Material material;
uniform Light light;

void main()
{
    // Vector pointing towards the light source.
    vec3 L = normalize(light.position - FragPos);

    // Normalized surface normal.
    vec3 N = normalize(Normal);

    // Reflect around the normal.
    vec3 R = normalize(reflect(-L, N));

    // Calculate the intensity over distance.
    float dist = length(light.position - FragPos);
    float atten = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    // Calculate diffuse and specular intensity.
    float diff = max(dot(L, N), 0.0);
    float spec = pow(max(dot(normalize(-FragPos), R), 0.0), material.shininess);

    vec3 ambient = atten * light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = atten * light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = atten * light.specular * spec * vec3(texture(material.specular, TexCoords));

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
