#version 330 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUv;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D mainTex;

out vec2 uv;
out vec3 normal;
out vec3 worldPosition;

void main()
{
	worldPosition = vec3(transform * vec4(vPos, 1.0));

	worldPosition.y += texture(mainTex, vUv).r * 300.0;

	gl_Position = projection * view * vec4(worldPosition, 1.0);

	uv = vUv;
	normal = vNormal;
}