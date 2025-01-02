#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

struct Wave {
    float amplitude;
    float timeConstant;
    vec2 Frequency;
    float offset;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
uniform float numVertices;
uniform Wave[10] waves;
uniform int nWaves;

void main()
{
    vec3 newPos = aPos;
    vec3 subNormal = vec3(0, 1, 0);
    for (int i=0; i<nWaves; i++) {
        newPos.y = newPos.y + waves[i].amplitude*sin(waves[i].offset + dot(aPos.xz, waves[i].Frequency) + waves[i].timeConstant * time);
        subNormal.y = subNormal.y + 1.0;
        subNormal.xz =  subNormal.xz - (waves[i].amplitude * waves[i].Frequency)*cos(waves[i].offset + dot(aPos.xz, waves[i].Frequency) + waves[i].timeConstant * time);
    }
    Normal = subNormal;
    FragPos = vec3(model * vec4(newPos, 1.0));
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}