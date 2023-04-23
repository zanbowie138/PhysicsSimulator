#version 330 core
out vec4 o_Color;

in vec3 Position;
in vec3 Color;
in vec3 Normal;

layout(std140) uniform Lighting 
{
	vec4 camPos;
	vec4 lightPos;
	vec4 lightColor;
};

vec4 pointLight() 
{
	vec3 lightVec = lightPos.xyz - Position;
	float dist = length(lightVec);
	float a = 0.05;
	float b = 0.7;
	float intensity = 1.0f / (a * dist * dist + b * dist + 1.0f);

	// Ambient Lighting
	float ambient = 0.2f;

	vec3 normal = normalize(Normal);
	vec3 camDir = normalize(camPos.xyz - Position);
	vec3 lightDir = normalize(lightVec);

	float diffuse = max(dot(normal, lightDir), 0.0f);

	// Specular lighting
	float specular = 0.0f;
	if (diffuse != 0.0f)
	{
		float specularLight = 0.50f;
		vec3 halfwayVec = normalize(camDir + lightDir);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularLight;
	};
	return vec4(Color, 1.0f) * (diffuse * intensity + ambient + specular) * lightColor;
}

void main()
{
	o_Color = pointLight();
}
