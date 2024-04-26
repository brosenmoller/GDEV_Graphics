#version 330 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 vUv;
layout(location = 3) in vec3 vNormal;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertexColor;
out vec2 uv;

void main()
{
	gl_Position = projection * view * transform * vec4(vPos, 1.0);

	vertexColor = vColor;
	uv = vUv;
}