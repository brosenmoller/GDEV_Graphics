#version 330 core
out vec4 FragColor;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;

in vec3 worldPosition;

vec3 lerp(vec3 a, vec3 b, float t)
{
	return a + (b - a) * t;
}

void main()
{
	vec3 topColor = vec3(68.0, 118.0, 189.0) / 255.0;
	vec3 bottomColor = vec3(188.0, 214.0, 231.0) / 255.0;
	vec3 sunColor = vec3(255.0, 200.0, 50.0) / 255.0;
 
	vec3 viewDirection = normalize(worldPosition - cameraPosition);

	float sun = max(pow(dot(-viewDirection, lightDirection), 128), 0.0);

	vec3 colorOut = lerp(bottomColor, topColor, abs(viewDirection.y)) + sun * sunColor;
	FragColor = vec4(colorOut, 1.0);
}