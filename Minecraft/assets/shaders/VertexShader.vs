#version 330 core

layout (location = 0) in vec3 iPos;
layout (location = 1) in vec2 iTextureCoords;

out vec2 oTextureCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * vec4( iPos, 1);
	oTextureCoords = iTextureCoords;
}
