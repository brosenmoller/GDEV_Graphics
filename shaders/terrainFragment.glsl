#version 330 core
out vec4 FragColor;

in vec2 uv;
in vec3 worldPosition;

uniform sampler2D mainTex;
uniform sampler2D normalTex;

uniform sampler2D dirtTex;
uniform sampler2D grassTex;
uniform sampler2D rockTex;
uniform sampler2D snowTex;
uniform sampler2D sandTex;

uniform vec3 lightDirection;
uniform vec3 cameraPosition;

vec4 lerp(vec4 a, vec4 b, float t)
{
	return a + (b - a) * t;
}

vec4 chooseCorrectColor(float height, float heightCutoff1, float heightCutoff2, 
                              float heightBlendCutoff1, float heightBlendCutoff2,
                              vec4 color0, vec4 color1, vec4 color2)
{
    float t0 = step(heightCutoff1 - heightBlendCutoff1, height);
    float t1 = step(heightCutoff1 + heightBlendCutoff1, height);
    float t2 = step(heightCutoff2 - heightBlendCutoff2, height);
    float t3 = step(heightCutoff2 + heightBlendCutoff2, height);

    vec4 colorBlend0_1 = lerp(color0, color1, clamp((height - (heightCutoff1 - heightBlendCutoff1)) / (2.0 * heightBlendCutoff1), 0.0, 1.0));
    vec4 colorBlend1_2 = lerp(color1, color2, clamp((height - (heightCutoff2 - heightBlendCutoff2)) / (2.0 * heightBlendCutoff2), 0.0, 1.0));

    vec4 finalColor = mix(color0, colorBlend0_1, t0 * (1.0 - t1));
    finalColor = mix(finalColor, color1, t1 * (1.0 - t2));
    finalColor = mix(finalColor, colorBlend1_2, t2 * (1.0 - t3));
    finalColor = mix(finalColor, color2, t3);

    return finalColor;
}

void main()
{
	vec3 normalMapNormal = texture(normalTex, uv).rgb;
	normalMapNormal = normalize(normalMapNormal * 2.0 - 1.0);
	normalMapNormal.gb = normalMapNormal.bg;
	normalMapNormal.g = -normalMapNormal.g;

	// Specular Data
//	vec3 viewDirection = normalize(worldPosition - cameraPosition);
//	vec3 reflectedLight = normalize(reflect(lightDirection, normalMapNormal));
//	float phongSpecular = pow(max(dot(reflectedLight, normalize(viewDirection)), 1.0), 128); // higher power = smaller highlight


	// Lighting
	float lightValue = min(max(dot(normalMapNormal, lightDirection), 0.0) + 0.1, 1.0); // Simple Diffuse Lighting
	//vec3 ambientLight = ambientLightColor * ambientLightIntensity; // Ambient

//	vec4 colorOutput = texture(dirtTex, uv);

    vec4 colorOutput = chooseCorrectColor(worldPosition.y, 10, 20, 30, 40, texture(sandTex, uv), texture(grassTex, uv), texture(snowTex, uv));

	colorOutput.rgb = colorOutput.rgb * lightValue;

	// Output the final color
	FragColor = colorOutput;
}
