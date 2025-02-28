#include "Buffers.h"

#include "include/glad/glad.h"

Vao::~Vao()
{
	glDeleteVertexArrays(1, &m_VaoID);
}

void Vao::Create()
{
	glGenVertexArrays(1, &m_VaoID);
	glBindVertexArray(m_VaoID);
}

void Vao::Bind()
{
	glBindVertexArray(m_VaoID);
}

void Vao::Unbind()
{ 
	glBindVertexArray(0); 
}



BufferObject::BufferObject(unsigned int type, unsigned int size)
	: m_BufferObjectID(0), m_Size(size), m_Type(type)
{
	glGenBuffers(1, &m_BufferObjectID);
	glBindBuffer(m_Type, m_BufferObjectID);
	glBufferData(m_Type, m_Size, nullptr, GL_STATIC_DRAW);
}

BufferObject::~BufferObject()
{
	glDeleteBuffers(1, &m_BufferObjectID);
}

void BufferObject::Create()
{
	glGenBuffers(1, &m_BufferObjectID);
}

void BufferObject::Load(unsigned int type, unsigned int size)
{
	m_Type = type;
	m_Size = size;

	glBindBuffer(m_Type, m_BufferObjectID);
	glBufferData(m_Type, m_Size, nullptr, GL_STATIC_DRAW);
}

void BufferObject::Bind()
{ 
	glBindBuffer(m_Type, m_BufferObjectID); 
}

void BufferObject::Unbind() 
{ 
	glBindBuffer(m_Type, 0); 
}





void BufferObject::SetData(void* data)
{
	Bind();
	glBufferData(m_Type, m_Size, data, GL_STATIC_DRAW);
}

