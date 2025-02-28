#pragma once

#include <iostream>
#include <string>

#include "include/glad/glad.h"

class Texture2D
{
public:
	Texture2D() = default;
	Texture2D(const std::string& filepath);
	~Texture2D();

	void Init(const std::string& filepath);

	void Bind() { glBindTexture(GL_TEXTURE_2D, m_TextureID); }
	void Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

	unsigned int GetWidth() const { return m_Width; }
	unsigned int GetHeight() const { return m_Height; }

private:
	unsigned int m_TextureID = 0;
	unsigned int m_Width = 0;
	unsigned int m_Height = 0;
};

