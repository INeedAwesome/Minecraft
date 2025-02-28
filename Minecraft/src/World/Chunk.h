#pragma once

#include <array>

#include "Block.h"
#include "Buffers.h"
#include "ChunkMesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "SimplexNoise.h"

#include <thread>

class Chunk
{
public:
	Chunk(const glm::vec3& position);
	~Chunk();
	void ClearChunk();
	
	glm::ivec3 GetPosition();

	void SetBlock(const glm::ivec3& position, BlockType type);
	BlockType GetBlock(const glm::vec3& position);

	void Update();
	void Render();
	void RegenerateChunkMesh();
	void GenerateBlocks(const SimplexNoise& noise);

	void SetHasChanged() { m_HasChanged = true; }

	// Used to generate the chunk, we need to check if we even can place the block face, that's what the AddFace function is used for
	void Generate(const SimplexNoise& noise);

private:
	glm::ivec3 m_Position;
	std::array<Block, CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH> m_Blocks;

	bool m_HasChanged = true;

	bool m_ReadyToGenerateMesh = false;
	bool m_HasGeneratedMesh = false;
	bool m_HasGeneratedAndFinishedBefore = false;

	ChunkMesh m_ChunkMesh;
	std::thread m_ChunkMeshBuilderThread;
	std::thread m_ChunkBuilderThread;


};

