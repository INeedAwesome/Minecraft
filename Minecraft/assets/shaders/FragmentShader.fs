#version 330 core

in vec2 oTextureCoords;

out vec4 FragColor;

uniform sampler2D blockTexture;

void main() 
{
	vec4 colorAndAlpha = texture(blockTexture, oTextureCoords);
	// FragColor = colorAndAlpha;
	FragColor = vec4(colorAndAlpha);
}
