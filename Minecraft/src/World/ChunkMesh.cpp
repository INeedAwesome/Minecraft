#include "ChunkMesh.h"

#include <glad/glad.h>

#include "Textures/TextureAtlas.h"

ChunkMesh::ChunkMesh()
	: m_ChunkPosition(0, 0, 0), m_Ebo()
{
	m_Vao.Create();
	m_Vbo.Create();
	m_Ebo.Create();
}

void ChunkMesh::Render()
{
	if (!m_HasUploadedToGPU)
		return;
	m_Vao.Bind();
	m_Vbo.Bind();
	m_Ebo.Bind();
	glDrawElements(GL_TRIANGLES, (GLsizei)m_Elements.size(), GL_UNSIGNED_INT, 0);
	m_Ebo.Unbind();
	m_Vbo.Unbind();
	m_Vao.Unbind();
}

void ChunkMesh::Destroy()
{
	m_Vertices.clear();
	m_Elements.clear();
}

void ChunkMesh::CreateMesh(std::array<Block, CHUNK_WIDTH* CHUNK_HEIGHT* CHUNK_DEPTH>& blocks, const glm::ivec3& chunkPosition)
{
	m_ReadyToSendToGPU = false;
	m_HasUploadedToGPU = false;
	Destroy();
	m_Vertices.reserve(CHUNK_WIDTH * CHUNK_DEPTH * 4); 
	m_Elements.reserve(CHUNK_WIDTH * CHUNK_DEPTH * 6);
	m_ChunkPosition = chunkPosition;

	int blockInChunk = 0; // what block are we on
	for (int ix = 0; ix < CHUNK_WIDTH; ix++)
	{
		for (int iy = 0; iy < CHUNK_HEIGHT; iy++)
		{
			for (int iz = 0; iz < CHUNK_DEPTH; iz++)
			{
				Block& block = blocks[blockInChunk];
				if (!block.IsOpaque())
				{
					if (GetBlock(blocks, { ix, iy, iz + 1 }).IsOpaque()) {
						AddFace(Front, block.blockType, { ix, iy, iz });
					}
					if (GetBlock(blocks, { ix, iy, iz - 1 }).IsOpaque()) {
						AddFace(Back, block.blockType, { ix, iy, iz });
					}

					if (GetBlock(blocks, { ix + 1, iy, iz }).IsOpaque()) {
						AddFace(Right, block.blockType, { ix, iy, iz });
					}
					if (GetBlock(blocks, { ix - 1, iy, iz }).IsOpaque()) {
						AddFace(Left, block.blockType, { ix, iy, iz });
					}

					if (GetBlock(blocks, { ix, iy + 1, iz }).IsOpaque()) {
						AddFace(Top, block.blockType, { ix, iy, iz });
					}
					if (GetBlock(blocks, { ix , iy - 1, iz }).IsOpaque()) {
						AddFace(Bottom, block.blockType, { ix, iy, iz });
					}
				}
				else if (block.blockType == Water && iy == 49)
				{
					if (GetBlock(blocks, { ix, iy+1, iz }).blockType == Air) {
						AddFace(Top, block.blockType, { ix, iy, iz });
					}
				}
				blockInChunk++; // goto next block
			}
		}
	}

	m_ReadyToSendToGPU = true;
}

void ChunkMesh::SendDataToGPU()
{
	m_Vao.Bind();

	// VERTICES DATA
	m_Vbo.Load(GL_ARRAY_BUFFER, (unsigned int)m_Vertices.size() * sizeof(Vertex));
	m_Vbo.SetData(m_Vertices.data());

	uint64_t stride = offsetof(Vertex, position);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)stride);
	glEnableVertexAttribArray(0);

	// TEXTURE COORDINATES
	stride = offsetof(Vertex, textureCoordinate);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)stride);
	glEnableVertexAttribArray(1);

	// INDICES DATA
	m_Ebo.Load(GL_ELEMENT_ARRAY_BUFFER, (unsigned int)m_Elements.size() * sizeof(unsigned int));
	m_Ebo.SetData(m_Elements.data());

	m_Vao.Unbind();

	glFlush();
	glFinish(); 

	m_HasUploadedToGPU = true;
}

void ChunkMesh::AddFace(BlockFace face, BlockType type, const glm::vec3 position)
{
	glm::vec3 bottomLeftForward	(( 0 + position.x) + CHUNK_WIDTH * m_ChunkPosition.x, (0 + position.y) + CHUNK_HEIGHT * m_ChunkPosition.y, ( 1 + position.z) + CHUNK_DEPTH * m_ChunkPosition.z);
	glm::vec3 bottomRightForward(( 1 + position.x) + CHUNK_WIDTH * m_ChunkPosition.x,(0 + position.y) + CHUNK_HEIGHT * m_ChunkPosition.y, ( 1 + position.z) + CHUNK_DEPTH * m_ChunkPosition.z);
	glm::vec3 topRightForward	(( 1  + position.x) + CHUNK_WIDTH * m_ChunkPosition.x,( 1 + position.y) + CHUNK_HEIGHT * m_ChunkPosition.y, ( 1 + position.z) + CHUNK_DEPTH * m_ChunkPosition.z);
	glm::vec3 topLeftForward	(( 0 + position.x) + CHUNK_WIDTH * m_ChunkPosition.x, ( 1 + position.y) + CHUNK_HEIGHT * m_ChunkPosition.y, ( 1 + position.z) + CHUNK_DEPTH * m_ChunkPosition.z);

	glm::vec3 bottomLeftBack	((0 + position.x) + CHUNK_WIDTH * m_ChunkPosition.x, (0 + position.y) + CHUNK_HEIGHT * m_ChunkPosition.y, (0 + position.z) + CHUNK_DEPTH * m_ChunkPosition.z);
	glm::vec3 bottomRightBack	(( 1 + position.x) + CHUNK_WIDTH * m_ChunkPosition.x, (0 + position.y) + CHUNK_HEIGHT * m_ChunkPosition.y, (0 + position.z) + CHUNK_DEPTH * m_ChunkPosition.z);
	glm::vec3 topRightBack		(( 1 + position.x) + CHUNK_WIDTH * m_ChunkPosition.x, ( 1 + position.y) + CHUNK_HEIGHT * m_ChunkPosition.y, (0 + position.z) + CHUNK_DEPTH * m_ChunkPosition.z);
	glm::vec3 topLeftBack		((0 + position.x) + CHUNK_WIDTH * m_ChunkPosition.x, ( 1 + position.y) + CHUNK_HEIGHT * m_ChunkPosition.y, (0 + position.z) + CHUNK_DEPTH * m_ChunkPosition.z);

	Vertex vertices[4]{};
	//std::array<Vertex, 4> vertices{};

	switch (face)
	{
	case Front:		
		vertices[0].position = bottomLeftForward; 
		vertices[1].position = bottomRightForward;
		vertices[2].position = topRightForward;
		vertices[3].position = topLeftForward;
		break;

	case Left:
		vertices[0].position = bottomLeftBack; 
		vertices[1].position = bottomLeftForward;
		vertices[2].position = topLeftForward;
		vertices[3].position = topLeftBack;
		break;

	case Right:
		vertices[0].position = bottomRightForward;
		vertices[1].position = bottomRightBack;
		vertices[2].position = topRightBack;
		vertices[3].position = topRightForward;
		break;

	case Back:
		vertices[0].position = bottomRightBack;
		vertices[1].position = bottomLeftBack;
		vertices[2].position = topLeftBack;
		vertices[3].position = topRightBack;
		break;

	case Top:
		vertices[0].position = topLeftForward;
		vertices[1].position = topRightForward;
		vertices[2].position = topRightBack;
		vertices[3].position = topLeftBack;
		break;	

	case Bottom:
		vertices[0].position = bottomLeftBack;
		vertices[1].position = bottomRightBack;
		vertices[2].position = bottomRightForward;
		vertices[3].position = bottomLeftForward;
		break;

	default:
		break;
	}

	float size = TextureAtlas::Get().GetTextureSize();
	float atlasSize = TextureAtlas::Get().GetAtlasSize();
	glm::vec2 offset = GetUV(type, face);

	vertices[0].textureCoordinate = glm::vec2(offset.x * size / atlasSize, offset.y * size / atlasSize);
	vertices[1].textureCoordinate = glm::vec2(((offset.x * size) + size) / atlasSize, offset.y * size / atlasSize);
	vertices[2].textureCoordinate = glm::vec2(((offset.x * size) + size) / atlasSize, ((offset.y * size) + size) / atlasSize);
	vertices[3].textureCoordinate = glm::vec2(offset.x * size / atlasSize, ((offset.y * size) + size) / atlasSize);

	m_Vertices.push_back(vertices[0]);
	m_Vertices.push_back(vertices[1]);
	m_Vertices.push_back(vertices[2]);
	m_Vertices.push_back(vertices[3]);

	int amountOfBlocks = (int)m_Elements.size() / 6; // This little bitch
	m_Elements.push_back(0 + (4 * amountOfBlocks));
	m_Elements.push_back(1 + (4 * amountOfBlocks));
	m_Elements.push_back(2 + (4 * amountOfBlocks));
	m_Elements.push_back(0 + (4 * amountOfBlocks));
	m_Elements.push_back(2 + (4 * amountOfBlocks));
	m_Elements.push_back(3 + (4 * amountOfBlocks));

}

Block ChunkMesh::GetBlock(const std::array<Block, CHUNK_VOLUME>& blocks, const glm::ivec3& position)
{
	if (position.x >= CHUNK_WIDTH || position.x < 0 || position.y >= CHUNK_HEIGHT || position.y < 0 || position.z >= CHUNK_DEPTH || position.z < 0)
	{
		return {};
	}
	int posInList = ((position.x * CHUNK_HEIGHT*CHUNK_DEPTH) + (position.y * CHUNK_DEPTH)) + position.z;
	return blocks[posInList];
}
