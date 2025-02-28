#include "Framebuffer.h"

#include "FileUtil.h"
#include <Window.h>

/*
* This is the vertices, they cover the whole screen, they are in screen coordinates
*/
static float framebufferVerticesAndTexCoords[] =
{
	-1.0f, -1.0f,		0.0f, 0.0f,
	 1.0f, -1.0f,		1.0f, 0.0f,
	 1.0f,  1.0f,		1.0f, 1.0f,

	-1.0f, -1.0f,		0.0f, 0.0f,
	 1.0f,  1.0f,		1.0f, 1.0f,
	-1.0f,  1.0f,		0.0f, 1.0f,
};

Framebuffer::~Framebuffer()
{
	glDeleteTextures(1, &m_ColorTexture);
	glDeleteTextures(1, &m_DepthTexture);

	glDeleteFramebuffers(1, &m_FramebufferObject);
}

bool Framebuffer::Init(int width, int height)
{
	m_FramebufferWidth = width;
	m_FramebufferHeight = height;

	std::string framebufferVertexSource = FileUtil::ReadFile("assets/shaders/FramebufferVertexShader.vs");
	std::string framebufferFragmentSource = FileUtil::ReadFile("assets/shaders/FramebufferFragmentShader.fs");
	m_FramebufferShader.Init(framebufferVertexSource, framebufferFragmentSource);
	
	// Generate the framebuffer
	glGenFramebuffers(1, &m_FramebufferObject);
	Bind();

	GenerateTextures();

	int status = CheckStatus();
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is incomplete! Status: " << status << std::endl;
		return false;
	}

	GenerateVAO();
	m_FramebufferVAO.Unbind();

	return true;
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferObject);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::SetUniform(const std::string& name, bool value)
{
	m_FramebufferShader.Bind();
	m_FramebufferShader.SetUniform(name, value);	
}
void Framebuffer::SetUniform(const std::string& name, int value)
{
	m_FramebufferShader.Bind();
	m_FramebufferShader.SetUniform(name, value);
}

void Framebuffer::DrawToScreen(Camera& camera)
{
	Unbind(); // Unbind the framebuffer to draw to the screen.

	glDisable(GL_DEPTH_TEST);

	m_FramebufferVAO.Bind();
	m_FramebufferShader.Bind();

	// Bind the color texture to texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_ColorTexture);

	// Bind the depth texture to texture unit 2
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);

	// Set the shader uniforms
	m_FramebufferShader.SetUniform("v_screenTexture", 1); // screenTexture is on GL_TEXTURE0
	m_FramebufferShader.SetUniform("v_depthTexture", 2);  // depthTexture is on GL_TEXTURE1

	m_FramebufferShader.SetUniform("v_projectionMatrix", camera.GetProjectionMatrix());	// We need these in the shader for fog
	m_FramebufferShader.SetUniform("v_viewMatrix", camera.GetViewMatrix());
	m_FramebufferShader.SetUniform("v_cameraPos", camera.GetPosition());

	glDrawArrays(GL_TRIANGLES, 0, 6); // Draw 

	m_FramebufferShader.Unbind();
	m_FramebufferVAO.Unbind();
}

void Framebuffer::Update(int width, int height)
{
	m_FramebufferWidth = width;
	m_FramebufferHeight = height;
	Bind();

	glDeleteTextures(1, &m_ColorTexture);
	GenerateTextures();

}

int Framebuffer::CheckStatus()
{
	Bind();
	return glCheckFramebufferStatus(GL_FRAMEBUFFER);
}

void Framebuffer::GenerateVAO()
{
	m_FramebufferVAO.Create();
	m_FramebufferVAO.Bind();

	m_FramebufferVBO.Create();
	m_FramebufferVBO.Load(GL_ARRAY_BUFFER, sizeof(framebufferVerticesAndTexCoords));
	m_FramebufferVBO.Bind();
	m_FramebufferVBO.SetData(framebufferVerticesAndTexCoords);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void Framebuffer::GenerateTextures()
{
	// Create color texture
	glGenTextures(1, &m_ColorTexture);
	glBindTexture(GL_TEXTURE_2D, m_ColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_FramebufferWidth, m_FramebufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture, 0);

	// https://community.khronos.org/t/using-multiple-gl-color-attachment-in-the-shader/108038
	// https://gamedev.stackexchange.com/questions/182549/opengl-color-attachments-dont-work

	// Create depth texture
	glGenTextures(1, &m_DepthTexture);
	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_FramebufferWidth, m_FramebufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::TakeScreenshot(int width, int height)
{
	unsigned int size = width * height * 4;
	unsigned char* data = new unsigned char[size];
	if (!data)
	{
		std::cout << "Could not create buffer with size '" << size << "'!" << std::endl;
		return;
	}
	memset(data, 0, size);
	Unbind();	// Read the screens pixels
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

	time_t now = time(0); tm ltm; localtime_s(&ltm, &now);
	std::string year = std::to_string(ltm.tm_year + 1900);
	std::string month = ltm.tm_mon + 1 < 10 ? "0" + std::to_string(ltm.tm_mon + 1) : std::to_string(ltm.tm_mon + 1);
	std::string day = ltm.tm_mday < 10 ? "0" + std::to_string(ltm.tm_mday) : std::to_string(ltm.tm_mday);
	std::string hour = ltm.tm_hour < 10 ? "0" + std::to_string(ltm.tm_hour) : std::to_string(ltm.tm_hour);
	std::string minute = ltm.tm_min < 10 ? "0" + std::to_string(ltm.tm_min) : std::to_string(ltm.tm_min);
	std::string second = ltm.tm_sec < 10 ? "0" + std::to_string(ltm.tm_sec) : std::to_string(ltm.tm_sec);

	std::string filename = "screenshots/" + year + "-" + month + "-" + day + " " + hour + "_" + minute + "_" + second + ".png";

	stbi_flip_vertically_on_write(1);
	int status = stbi_write_png(filename.c_str(), width, height, 4, (void*)data, 0);
	if (status == 0)
	{
		std::cout << "Could not write the screenshot to disk!" << std::endl;
	}
	else
		std::cout << "Took screenshot at: " << filename << std::endl;

	delete[] data;
}


