#pragma once

#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Buffers.h"
#include "Block.h"

#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 256
#define CHUNK_DEPTH 32
#define CHUNK_VOLUME CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH

struct Vertex {
	glm::vec3 position;
	glm::vec2 textureCoordinate;
};

class ChunkMesh
{
public:
	ChunkMesh();
	~ChunkMesh() = default;

	void Render();
	void Destroy();

	void CreateMesh(std::array<Block, CHUNK_WIDTH* CHUNK_HEIGHT* CHUNK_DEPTH>& blocks, const glm::vec3& chunkPosition);
	void SendDataToGPU();

	bool IsReadyForGPUUpload() const { return m_ReadyToSendToGPU; }
	bool HasUploadedToGPU() const { return m_HasUploadedToGPU; }

private:

	void AddFace(BlockFace face, BlockType type, const glm::vec3 position);
	Block GetBlock(const std::array<Block, CHUNK_VOLUME>& blocks, const glm::vec3& position);

private: 
	glm::vec3 m_ChunkPosition;
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Elements;
	Vao m_Vao;
	BufferObject m_Vbo;
	BufferObject m_Ebo;

	bool m_ReadyToSendToGPU = false;
	bool m_HasUploadedToGPU = false;

};
