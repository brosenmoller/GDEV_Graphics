#version 330 core
out vec4 FragColor;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;

uniform vec3 topColorValue;
uniform vec3 middleColorValue;
uniform vec3 bottomColorValue;
uniform vec3 sunColorValue;

in vec3 worldPosition;

vec3 lerp(vec3 a, vec3 b, float t)
{
	return a + (b - a) * t;
}

void main()
{
	vec3 topColor = topColorValue / 255.0;
	vec3 middleColor = middleColorValue / 255.0;
	vec3 bottomColor = bottomColorValue / 255.0;
	vec3 sunColor = sunColorValue / 255.0;

	vec3 viewDirection = normalize(worldPosition - cameraPosition);

	float sun = max(pow(dot(-viewDirection, lightDirection), 128), 0.0);

	float blendFactor = smoothstep(-0.001, 0.001, viewDirection.y); // Smooth transition at y=0
	vec3 selectedColor = mix(bottomColor, topColor, blendFactor); // Mix based on blendFactor

	vec3 colorOut = lerp(middleColor, selectedColor, pow(abs(viewDirection.y), 0.7)) + sun * sunColor;
	

	FragColor = vec4(colorOut, 1.0);
}