#pragma once

#include <string>

#include "Shader.h"
#include "Buffers.h"
#include "Camera.h"

#include "stb_image_write.h"

class Framebuffer
{
public:
	Framebuffer() = default;
	~Framebuffer();

	bool Init(int width, int height);

	void Bind();
	void Unbind();

	void SetUniform(const std::string& name, bool value);
	void SetUniform(const std::string& name, int value);

	void DrawToScreen(Camera& camera);
	void Update(int width, int height);
	
	void TakeScreenshot(int width, int height);

private:
	int CheckStatus();

	void GenerateVAO();
	void GenerateTextures();


private:
	Shader m_FramebufferShader;
	Vao m_FramebufferVAO;
	BufferObject m_FramebufferVBO;
	unsigned int m_FramebufferObject;

	unsigned int m_FramebufferWidth;
	unsigned int m_FramebufferHeight;
	
	unsigned int m_ColorTexture;
	unsigned int m_DepthTexture;

};

