#pragma once

class Vao
{
public:
	Vao() = default;
	~Vao();
	void Create();
	void Bind();
	void Unbind();
private:
	unsigned int m_VaoID;
};

class BufferObject
{
public:
	BufferObject() = default;
	BufferObject(unsigned int type, unsigned int size);
	~BufferObject();

	void Create();
	void Load(unsigned int type, unsigned int size);
	void Bind();
	void Unbind();
	void SetData(void* data);

private:
	unsigned int m_BufferObjectID;
	unsigned int m_Size;
	unsigned int m_Type;
};

