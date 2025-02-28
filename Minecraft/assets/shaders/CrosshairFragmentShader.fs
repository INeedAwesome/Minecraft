#version 330 core

in vec2 oTexCoords;

out vec4 FragColor;

uniform vec2 iResolution;
uniform sampler2D crosshairTexture;

void main()
{
	float scale = 0.01;

	// Calculate texture coordinates to keep the crosshair in the center
	vec2 center = vec2(0.5, 0.5);
	vec2 uv = gl_FragCoord.xy / iResolution;

   // Preserve aspect ratio
	float aspectRatio = iResolution.x / iResolution.y;
	vec2 scaleFactor = vec2(scale, scale * aspectRatio);

	// Adjust texture coordinates to scale the crosshair without distortion
	vec2 texCoords = (uv - center) / scaleFactor + vec2(0.5, 0.5);

	vec4 texColor = texture(crosshairTexture, texCoords);

	// Render only the non-transparent parts of the texture
	if (texColor.a > 0.0)
	{
		FragColor = texColor;
	}
	else
	{
		discard; // Make the background transparent
	}
}
