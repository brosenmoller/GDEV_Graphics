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

void main()
{
	vec3 normalMapNormal = texture(normalTex, uv).rgb;
	normalMapNormal = normalize(normalMapNormal * 2.0 - 1.0);
	normalMapNormal = TBN * normalMapNormal;

	// Lighting
	vec3 lightDirection = normalize(lightPosition - worldPosition);
	float lightValue = max(dot(normalMapNormal, lightDirection), 0.0); // Simple Lighting
	lightValue = min(lightValue + 0.1, 1.0); // Ambient

	FragColor = vec4(vertexColor, 1.0) * texture(mainTex, uv) * lightValue;
}