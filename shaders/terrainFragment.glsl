#version 330 core

// Noise Methods: https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}
vec2 fade(vec2 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

float cnoise(vec2 P){
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod(Pi, 289.0); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;
  vec4 i = permute(permute(ix) + iy);
  vec4 gx = 2.0 * fract(i * 0.0243902439) - 1.0; // 1/41 = 0.024...
  vec4 gy = abs(gx) - 0.5;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;
  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);
  vec4 norm = 1.79284291400159 - 0.85373472095314 * 
    vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;
  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));
  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}
// end noise methods

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

float getGradientHeight(float blendNoiseScale, float blendNoiseMultiplier)
{
    vec2 xz = worldPosition.xz;
    float noiseOffset = cnoise(xz * blendNoiseScale) * 2.0 - 1.0;
    noiseOffset *= blendNoiseMultiplier;
    return noiseOffset + worldPosition.y;
}

float roundedCornerLerp(float value, float blendFactor, float blendPower)
{
    return 1.0 - pow(1.0 - pow(value, blendFactor), blendPower);
}

vec3 colorFromNormalWithLerp(vec3 normal, float slopeCutoff, vec3 color1, vec3 color2, float slopeBlendPower, float slopeBlendFactor)
{
    float value = (dot(normal, vec3(0,1,0)) - slopeCutoff + 1) / 2;
    value = roundedCornerLerp(value,slopeBlendFactor, slopeBlendPower);

    value = clamp(value, 0.0, 1.0);

    return mix(color1, color2, value);
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
    float y = getGradientHeight(.003, -20);
    float dirtToSand = clamp((y - 50) / 10, -1, 1) * .5 + .5;
    float sandToGrass = clamp((y - 100) / 10, -1, 1) * .5 + .5;
    float grassToSnow = clamp((y - 200) / 10, -1, 1) * .5 + .5;

    float dist = distance(worldPosition, cameraPosition);

    float uvScale = 10;
    vec3 dirtColor = texture(dirtTex, uv * uvScale).rgb;
    vec3 sandColor = texture(sandTex, uv * uvScale).rgb;
    vec3 grassColor = texture(grassTex, uv * uvScale).rgb;
    vec3 rockColor = texture(rockTex, uv * uvScale).rgb;
    vec3 snowColor = texture(snowTex, uv * uvScale).rgb;

    vec3 diffuse = lerp(dirtColor, sandColor, dirtToSand);
    diffuse = lerp(diffuse, grassColor, sandToGrass);
    diffuse = lerp(diffuse, snowColor, grassToSnow);

    diffuse = colorFromNormalWithLerp(normalMapNormal, 0.4, diffuse, rockColor, 60, 4);


    // Construct Color
    vec4 colorOutput = vec4(diffuse, 1.0);
	colorOutput.rgb = colorOutput.rgb * lightValue;

    vec3 topColor = vec3(68.0, 118.0, 189.0) / 255.0;
	vec3 middleColor = vec3(188.0, 214.0, 231.0) / 255.0;
	vec3 bottomColor = vec3(20.0, 20.0, 21.0) / 255.0;

    vec3 viewDirection = normalize(worldPosition - cameraPosition);

    float blendFactor = smoothstep(-0.001, 0.001, viewDirection.y); // Smooth transition at y=0
	vec3 selectedColor = mix(bottomColor, topColor, blendFactor); // Mix based on blendFactor

	vec3 fogColor = lerp(middleColor, selectedColor, pow(abs(viewDirection.y), 0.7));

    float fog = pow(clamp((dist - 2500) / 1000, 0, 1), 1.5);
    colorOutput.rgb = lerp(colorOutput.rgb, fogColor, fog);

	// Output the final color
	FragColor = colorOutput;
}
