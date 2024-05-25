#version 330 core
layout(location = 0) in vec3 vPos;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

out vec3 worldPosition;

void main()
{
	worldPosition = vec3(transform * vec4(vPos, 1.0));
	gl_Position = projection * view * vec4(worldPosition, 1.0);
}