#version 330 core
out vec4 FragColor;

in vec3 vertexColor;
in vec2 uv;

uniform sampler2D mainTex;

void main()
{
	FragColor = vec4(vertexColor, 1.0) * texture(mainTex, uv);
}