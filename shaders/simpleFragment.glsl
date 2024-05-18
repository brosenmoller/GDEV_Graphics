#version 330 core
out vec4 FragColor;

in vec3 vertexColor;
in vec2 uv;
in vec3 normal;
in mat3 TBN;
in vec3 worldPosition;

uniform sampler2D mainTex;
uniform sampler2D normalTex;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

void main()
{
	vec3 lightDirection = normalize(lightPosition - worldPosition);

	vec3 normalMapNormal = texture(normalTex, uv).rgb;
	normalMapNormal = normalize(normalMapNormal * 2.0 - 1.0);

	// scale normal down
	normalMapNormal.rg = normalMapNormal.rg * 0.7;
	normalMapNormal = normalize(normalMapNormal);

	// transform with tbn
	normalMapNormal = TBN * normalMapNormal;

	// Specular Data
	vec3 viewDirection = normalize(worldPosition - cameraPosition);
	vec3 reflectedLight = normalize(reflect(lightDirection, normalMapNormal));
	float phongSpecular = pow(max(dot(reflectedLight, normalize(viewDirection)), 0.0), 256); // higher power = smaller highlight

	// Lighting
	float lightValue = max(dot(normalMapNormal, lightDirection), 0.0); // Simple Lighting
	lightValue = min(lightValue + 0.1, 1.0); // Ambient

	vec4 colorOutput = vec4(vertexColor, 1.0) * texture(mainTex, uv);
	colorOutput.rgb = colorOutput.rgb * lightValue + phongSpecular * colorOutput.rgb;

	FragColor = colorOutput;
}