#version 330 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

uniform sampler2D mainTex;
uniform sampler2D normalTex;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;

void main()
{
	//vec3 normalMapNormal = texture(normalTex, uv).rgb;
	//normalMapNormal = normalize(normalMapNormal * 2.0 - 1.0);

	// Specular Data
//	vec3 viewDirection = normalize(worldPosition - cameraPosition);
//	vec3 reflectedLight = normalize(reflect(lightDirection, normalMapNormal));
//	float phongSpecular = pow(max(dot(reflectedLight, normalize(viewDirection)), 1.0), 128); // higher power = smaller highlight

	// Lighting
	float lightValue = min(max(dot(normal, lightDirection), 0.0) + 0.1, 1.0); // Simple Diffuse Lighting
	//vec3 ambientLight = ambientLightColor * ambientLightIntensity; // Ambient

	vec4 colorOutput = texture(mainTex, uv);
	colorOutput.rgb = colorOutput.rgb;

	// Output the final color
	FragColor = colorOutput;
}