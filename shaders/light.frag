#version 330 core
out vec4 o_Color;

uniform vec4 lightColor;

void main()
{
	o_Color = lightColor;
}
