#version 330 core
out vec4 o_Color;

in vec3 Position;
in vec3 Color;
in vec3 Normal;
in vec4 FragPosLightSpace;

layout(std140) uniform Lighting
{
	vec4 camPos;
	vec4 lightPos;
	vec4 lightColor;
};

uniform sampler2D shadowMap;

float shadowFactor(vec4 fragPosLS)
{
	vec3 proj = fragPosLS.xyz / fragPosLS.w * 0.5 + 0.5;
	if (proj.z > 1.0) return 1.0;
	float bias = max(0.005 * (1.0 - dot(normalize(Normal), normalize(lightPos.xyz - Position))), 0.0005);
	float shadow = 0.0;
	vec2 texel = 2.0 / textureSize(shadowMap, 0);
	for (int x = -2; x <= 2; ++x)
		for (int y = -2; y <= 2; ++y)
			shadow += (proj.z - bias > texture(shadowMap, proj.xy + vec2(x,y)*texel).r) ? 1.0 : 0.0;
	return 1.0 - shadow / 25.0;
}

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

	float shadow = shadowFactor(FragPosLightSpace);
	return vec4(Color, 1.0f) * (diffuse * intensity * shadow + ambient + specular * intensity * shadow) * lightColor;
}

void main()
{
	o_Color = pointLight();
}
