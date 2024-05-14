#version 330 core
out vec4 FragColor;

in vec3 vertexColor;
in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

uniform sampler2D mainTex;
uniform vec3 lightPosition;

void main()
{
	// Lighting
	vec3 lightDirection = normalize(lightPosition - worldPosition);
	float lightValue = max(dot(normal, lightDirection), 0.0); // Simple Lighting
	lightValue = min(lightValue + 0.1, 1.0); // Ambient

	FragColor = vec4(vertexColor, 1.0) * texture(mainTex, uv) * lightValue;
}