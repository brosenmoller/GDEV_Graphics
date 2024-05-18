#version 330 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 vUv;
layout(location = 3) in vec3 vNormal;
layout(location = 4) in vec3 vTangent;
layout(location = 5) in vec3 vBitangent;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertexColor;
out vec2 uv;
out vec3 normal;
out mat3 TBN;
out vec3 worldPosition;

void main()
{
	gl_Position = projection * view * transform * vec4(vPos, 1.0);

	vertexColor = vColor;
	uv = vUv;

	normal = normalize(mat3(transform) * vNormal);
	vec3 t = normalize(mat3(transform) * vTangent);
	vec3 b = normalize(mat3(transform) * vBitangent);
	TBN = mat3(t, b, normal);

	worldPosition = mat3(transform) * vPos;
}