#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Position;
out vec3 Color;
out vec3 Normal;

layout(std140) uniform Camera 
{
	mat4 camMatrix;
};

uniform mat4 model;
uniform vec3 color;

void main()
{
	Position = vec3(model * vec4(aPos, 1.0f));
	Normal = aNormal;
	Color = color;

	gl_Position = camMatrix * vec4(Position, 1.0f);
}
