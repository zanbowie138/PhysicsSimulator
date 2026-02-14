#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

out vec3 Position;
out vec3 Normal;
out vec2 texCoord;
out vec4 FragPosLightSpace;

layout(std140) uniform Camera
{
    mat4 camMatrix;
};
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    Position = vec3(model * vec4(aPos, 1.0f));
    Normal = aNormal;
    texCoord = aTex;
    FragPosLightSpace = lightSpaceMatrix * vec4(Position, 1.0);

    gl_Position = camMatrix * vec4(Position, 1.0f);
}
