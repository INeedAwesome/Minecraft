#version 330 core

layout (location = 0) in vec2 iPos;
layout (location = 1) in vec2 iTexCoords;

out vec2 oTexCoords;

uniform vec2 iResolution; // Screen resolution
// uniform float scale = 0.1; // Scale factor for the crosshair

void main()
{
	float scale = 1;
	// Calculate the center of the screen
	 vec2 center = vec2(0.5, 0.5) * iResolution;

    // Scale the position around the center
    vec2 scaledPos = (iPos - center) * scale + center;

    gl_Position = vec4((scaledPos ) * 2.0 - 1.0, 0.0, 1.0);

	oTexCoords = iTexCoords;
}
