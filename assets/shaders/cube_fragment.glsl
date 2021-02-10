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

    // Calculate diffuse and specular intensity.
    float diff = max(dot(L, N), 0.0);
    float spec = pow(max(dot(normalize(-FragPos), R), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
