#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec3 aColor;
//layout (location = 3) in vec2 aTex;

out vec3 Position;
out vec3 Color;
out vec3 Normal;
//out vec2 texCoord;

uniform mat4 camMatrix;
uniform mat4 model;

void main()
{
	Position = vec3(model * vec4(aPos, 1.0f));
	Normal = aNormal;
	Color = vec3(0.8f, 0.8f, 0.1f);
	//texCoord = aTex;

	gl_Position = camMatrix * vec4(Position, 1.0f);
}
