#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform sampler2D tex;


void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * texture(tex, TexCoord).rgb;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * texture(tex, TexCoord).rgb;

    vec3 result = (ambient + diffuse);
    FragColor = vec4(result, 1.0);
}