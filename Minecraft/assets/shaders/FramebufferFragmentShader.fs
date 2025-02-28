#version 330 core

in vec2 oTexCoords;

out vec4 FragColor;

uniform mat4 v_viewMatrix;
uniform mat4 v_projectionMatrix;
uniform vec3 v_cameraPos; // Camera position in world space

uniform bool v_useFog;
uniform sampler2D v_screenTexture;
uniform sampler2D v_depthTexture;
uniform int v_viewDistance;

vec4  fog_colour = vec4(0.2, 0.6, 0.8, 1.0);

// Function to reconstruct world position from depth
vec3 ReconstructWorldPos(vec2 texCoords, float depth)
{
	float z = depth * 2.0 - 1.0; // Convert to NDC (-1 to 1)
	
	vec4 clipSpacePos = vec4(texCoords * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePos = inverse(v_projectionMatrix) * clipSpacePos;
	viewSpacePos /= viewSpacePos.w;
	
	vec4 worldSpacePos = inverse(v_viewMatrix) * viewSpacePos;
	return worldSpacePos.xyz;
}

void main()
{
	float fog_mindist = (v_viewDistance * 32) - ((v_viewDistance-1) * 32); // Where fog starts 1 chunk from fog end 
	float fog_maxdist = (v_viewDistance * 32) - 32; // v_viewDistance in chunks to blocks, where fog ends
	
	vec4 color = texture(v_screenTexture, oTexCoords);
	float depth = texture(v_depthTexture, oTexCoords).r;

	vec3 worldPos = ReconstructWorldPos(oTexCoords, depth);

	float dist = length(worldPos - v_cameraPos);
	float fog_factor = (fog_maxdist - dist) / (fog_maxdist - fog_mindist);
	fog_factor = clamp(fog_factor, 0.0, 1.0);

	if (v_useFog)
	{
		FragColor = mix(fog_colour, color, fog_factor);
	}
	else 
	{
		FragColor = color;
	}
	
}
