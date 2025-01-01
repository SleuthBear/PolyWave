#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
uniform float numVertices;

void main()
{
    vec3 newPos = aPos;
    newPos.y = 0.05*sin(5*aPos.x + 2*time) + 0.05*sin(4*aPos.z + 1*time);// + 0.005*sin(400*aPos.x + 3*time);
    vec3 biNormal = vec3(1, 0, (0.05*5)*cos(5*aPos.x + 2*time)) ;
    vec3 tangent = vec3(-(0.05*4)*sin(4*aPos.z + 1*time), 0, 1);
    Normal = cross(tangent, biNormal);

    FragPos = vec3(model * vec4(newPos, 1.0));
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}