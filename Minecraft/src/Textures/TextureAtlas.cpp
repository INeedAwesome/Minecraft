#include "TextureAtlas.h"

void TextureAtlas::Bind()
{
	m_Atlas.Bind();
}

void TextureAtlas::Unbind()
{
	m_Atlas.Unbind();
}

float TextureAtlas::GetAtlasSize()
{
	return (float)m_Atlas.GetWidth();
}

float TextureAtlas::GetTextureSize()
{
	return (float)m_TextureSize;
}
