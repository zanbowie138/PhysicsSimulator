#version 330 core
layout (location = 0) in vec3 aPos;

layout(std140) uniform Camera 
{
	mat4 camMatrix;
};

uniform mat4 model;

void main()
{

	gl_Position = camMatrix * vec4(vec3(model * vec4(aPos, 1.0)),1.0);
	gl_PointSize = 10.0;
}
