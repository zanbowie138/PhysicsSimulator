#version 330 core
out vec4 o_Color;

in vec3 Position;
in vec3 Color;
in vec3 Normal;
in vec2 texCoord;

uniform sampler2D diffuse;
uniform sampler2D specular;

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

	return lightColor * (texture(diffuse0, texCoord) * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity);
}

/*
vec4 pointLight() 
{
	vec3 lightVec = lightPos - Position;
	float dist = length(lightVec);
	float a = 0.05;
	float b = 0.7;
	float intensity = 1.0f / (a * dist * dist + b * dist + 1.0f);

	float ambient = 0.2f;

	vec3 normal = normalize(Normal);
	vec3 camDir = normalize(camPos - Position);
	vec3 lightDir = normalize(lightVec);

	float specularLight = 0.5f;
	float diffuse =  max(dot(normal, lightDir),0.0f);
	float specular = pow(max(dot(reflect(-lightDir, normal), camDir),0),16) * specularLight;

	return (texture(diffuse0, texCoord) * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity) * lightColor;
}

vec4 directLight()
{

	float ambient = 0.2f;

	vec3 normal = normalize(Normal);
	vec3 camDir = normalize(camPos - Position);
	vec3 lightDir = normalize(vec3(1.0f, 1.0f, 0.0f));

	float specularLight = 0.5f;
	float diffuse =  max(dot(normal, lightDir),0.0f);
	float specular = pow(max(dot(reflect(-lightDir, normal), camDir),0),16) * specularLight;

	return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
}

vec4 spotLight()
{
	float outerCone = 0.9f;
	float innerCone = 0.95f;


	float ambient = 0.2f;

	vec3 normal = normalize(Normal);
	vec3 camDir = normalize(camPos - Position);
	vec3 lightDir = normalize(lightPos - Position);

	float specularLight = 0.5f;
	float diffuse =  max(dot(normal, lightDir),0.0f);
	float specular = pow(max(dot(reflect(-lightDir, normal), camDir),0),16) * specularLight;

	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDir);
	float intensity = clamp((angle - innerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(diffuse0, texCoord) * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity) * lightColor;
}*/

void main()
{
	o_Color = pointLight();
}
