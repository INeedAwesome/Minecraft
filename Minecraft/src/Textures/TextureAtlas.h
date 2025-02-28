#pragma once

#include "Texture2D.h"

class TextureAtlas
{
public:
	void Bind();
	void Unbind(); 

	float GetAtlasSize();
	float GetTextureSize();

public:
	TextureAtlas(TextureAtlas const&) = delete;
	void operator=(TextureAtlas const&) = delete;
	
	static TextureAtlas& Get() 
	{
		static TextureAtlas instance;
		return instance;
	}
private: 
	TextureAtlas() 
		: m_Atlas("assets/atlas.png"), m_TextureSize(m_Atlas.GetWidth() / m_AmountTexturesWidth)
	{};

private:
	Texture2D m_Atlas;
	std::string m_Filepath;
	int m_AmountTexturesWidth = 8;
	float m_TextureSize;


};

