#version 330 core
out vec4 o_Color;

in vec3 Color;

void main()
{
	o_Color = vec4(Color, 1.0);
}
