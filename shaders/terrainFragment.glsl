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

vec3 lerp(vec3 a, vec3 b, float t)
{
	return a + (b - a) * t;
}

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

	// Lighting
	float lightValue = min(max(dot(normalMapNormal, lightDirection), 0.0) + 0.1, 1.0); // Simple Diffuse Lighting

    // Terrain Color
    float y = worldPosition.y;
    float dirtToSand = clamp((y - 50) / 10, -1, 1) * .5 + .5;
    float sandToGrass = clamp((y - 100) / 10, -1, 1) * .5 + .5;
    float grassToRock = clamp((y - 150) / 10, -1, 1) * .5 + .5;
    float rockToSnow = clamp((y - 200) / 10, -1, 1) * .5 + .5;

    float dist = distance(worldPosition, cameraPosition);

    float uvScale = 10;
    vec3 dirtColor = texture(dirtTex, uv * uvScale).rgb;
    vec3 sandColor = texture(sandTex, uv * uvScale).rgb;
    vec3 grassColor = texture(grassTex, uv * uvScale).rgb;
    vec3 rockColor = texture(rockTex, uv * uvScale).rgb;
    vec3 snowColor = texture(snowTex, uv * uvScale).rgb;

    vec3 diffuse = lerp(dirtColor, sandColor, dirtToSand);
    diffuse = lerp(diffuse, grassColor, sandToGrass);
    diffuse = lerp(diffuse, rockColor, grassToRock);
    diffuse = lerp(diffuse, snowColor, rockToSnow);


    // Construct Color
    vec4 colorOutput = vec4(diffuse, 1.0);// chooseCorrectColor(worldPosition.y, 0, 50, 30, 30, texture(sandTex, uv), texture(grassTex, uv), texture(snowTex, uv));
	colorOutput.rgb = colorOutput.rgb * lightValue;

    vec3 topColor = vec3(68.0, 118.0, 189.0) / 255.0;
	vec3 middleColor = vec3(188.0, 214.0, 231.0) / 255.0;
	vec3 bottomColor = vec3(20.0, 20.0, 21.0) / 255.0;

    vec3 viewDirection = normalize(worldPosition - cameraPosition);

    float blendFactor = smoothstep(-0.001, 0.001, viewDirection.y); // Smooth transition at y=0
	vec3 selectedColor = mix(bottomColor, topColor, blendFactor); // Mix based on blendFactor

	vec3 fogColor = lerp(middleColor, selectedColor, pow(abs(viewDirection.y), 0.7));

    float fog = pow(clamp((dist - 1500) / 1000, 0, 1), 1.5);
    colorOutput.rgb = lerp(colorOutput.rgb, fogColor, fog);

	// Output the final color
	FragColor = colorOutput;
}
