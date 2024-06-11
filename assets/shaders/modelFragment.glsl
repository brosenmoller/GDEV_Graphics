#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;
in vec4 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_ao1;

uniform vec3 cameraPosition;
uniform vec3 lightDirection;

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

vec3 lerp(vec3 a, vec3 b, float t)
{
	return a + (b - a) * t;
}

vec4 lerp(vec4 a, vec4 b, float t)
{
	return a + (b - a) * t;
}

void main()
{
    vec4 diffuse = texture(texture_diffuse1, TexCoords);
    vec4 specTex = texture(texture_specular1, TexCoords);

    float light = max(dot(-lightDirection, Normals), 0.0);

    vec3 viewDir = normalize(cameraPosition - FragPos.rgb);
    vec3 refl = reflect(lightDirection, Normals);

    float ambientOcclusion = texture(texture_ao1, TexCoords).r;
    
    float roughness = texture(texture_roughness1, TexCoords).r;
    float spec = pow(max(dot(viewDir, refl), 0.0), lerp(1, 128, roughness));
    vec3 specular = spec * specTex.rgb;

    vec4 finalColor = diffuse * max(light * ambientOcclusion, 0.2 * ambientOcclusion) + vec4(specular, 0);

    // FOg
    float dist = distance(FragPos.rgb, cameraPosition);

    vec3 topColor = vec3(68.0, 118.0, 189.0) / 255.0;
	vec3 middleColor = vec3(188.0, 214.0, 231.0) / 255.0;
	vec3 bottomColor = vec3(20.0, 20.0, 21.0) / 255.0;

    float blendFactor = smoothstep(-0.001, 0.001, -viewDir.y); // Smooth transition at y=0
	vec3 selectedColor = mix(bottomColor, topColor, blendFactor); // Mix based on blendFactor

	vec3 fogColor = lerp(middleColor, selectedColor, pow(abs(-viewDir.y), 0.7));

    float fog = pow(clamp((dist - 2500) / 1000, 0, 1), 1.5);
    
    vec4 tmpOutput = lerp(finalColor, vec4(fogColor, 1.0), fog);
    
    //Clip at threshold
    if(tmpOutput.a < 0.01) {
        discard;
    }

    FragColor = tmpOutput;
}