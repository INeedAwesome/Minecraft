#include "Chunk.h"

#include "include/glad/glad.h"

#include <thread>

Chunk::Chunk(const glm::vec3& position)
	: m_Position(position)
{
}

Chunk::~Chunk()
{
	ClearChunk();
	if (m_ChunkBuilderThread.joinable())
		m_ChunkBuilderThread.join();

	if (m_ChunkMeshBuilderThread.joinable())
		m_ChunkMeshBuilderThread.join();
}

void Chunk::ClearChunk()
{
	if (m_ChunkMeshBuilderThread.joinable())
		m_ChunkMeshBuilderThread.join();
	m_ChunkMesh.Destroy();
}

glm::vec3 Chunk::GetPosition()
{
	return m_Position;
}

void Chunk::SetBlock(const glm::vec3& position, BlockType type)
{
	glm::vec3 local_pos = glm::vec3(glm::floor(position.x), glm::floor(position.y), glm::floor(position.z));
	int posInList = ((local_pos.x * CHUNK_HEIGHT * CHUNK_DEPTH) + (local_pos.y * CHUNK_DEPTH)) + local_pos.z;
	m_Blocks[posInList].blockType = type;
	m_HasChanged = true;
}

BlockType Chunk::GetBlock(const glm::vec3& position)
{
	glm::vec3 local_pos = glm::vec3(glm::floor(position.x), glm::floor(position.y), glm::floor(position.z));
	int posInList = ((local_pos.x * CHUNK_HEIGHT * CHUNK_DEPTH) + (local_pos.y * CHUNK_DEPTH)) + local_pos.z;
	return m_Blocks[posInList].blockType;
}

void Chunk::Update()
{

	if (m_ReadyToGenerateMesh && !m_HasGeneratedMesh)
	{
		if (m_ChunkMeshBuilderThread.joinable())
			m_ChunkMeshBuilderThread.join();
		m_HasGeneratedMesh = true;
		m_ReadyToGenerateMesh = false;
		RegenerateChunkMesh();
	}

	if (m_HasGeneratedMesh && m_ChunkMesh.IsReadyForGPUUpload() && !m_ChunkMesh.HasUploadedToGPU())
	{
		if (m_ChunkMeshBuilderThread.joinable())
			m_ChunkMeshBuilderThread.join();
		m_ChunkMesh.SendDataToGPU();
		m_HasGeneratedMesh = true;
	}
}

void Chunk::Render()
{
	m_ChunkMesh.Render();
}

void Chunk::RegenerateChunkMesh()
{
	if (!m_HasGeneratedAndFinishedBefore)
	{
		m_ChunkMeshBuilderThread = std::thread(&ChunkMesh::CreateMesh, &m_ChunkMesh, std::ref(m_Blocks), std::ref(m_Position));
		//m_ChunkMeshBuilderThread.join();
		m_ChunkMeshBuilderThread.detach();
		//m_ChunkMesh.CreateMesh(m_Blocks, m_Position);
		//m_ChunkMesh.SendDataToGPU();
		m_HasGeneratedAndFinishedBefore = true;
	}
	else
	{
		m_ChunkMesh.CreateMesh(m_Blocks, m_Position);
		m_ChunkMesh.SendDataToGPU();
		m_HasGeneratedMesh = true;
		
	}

	m_HasChanged = false;
	
}

void Chunk::GenerateBlocks(const SimplexNoise& noise)
{
	m_ReadyToGenerateMesh = false;
	int waterLevel = 50;

	int blockWereOn = 0;

	for (int x = 0; x < CHUNK_WIDTH; x++)
	{
		for (int z = 0; z < CHUNK_DEPTH; z++)
		{
			float amplitude = 20.0f;
			float amplitudeExtreme = 40.0f;
			float scale = 0.005f;

			float bx = x + (m_Position.x * CHUNK_DEPTH);
			bx *= scale;
			float bz = z + (m_Position.z * CHUNK_WIDTH);
			bz *= scale;

			float elevation = glm::abs(noise.noise(bx, bz) * amplitude);
			elevation += (glm::abs(noise.noise(glm::sin(bx * 2), bz * 2) * (amplitude / 8))) / 2;
			elevation += noise.noise(glm::sin(bx * 0.3f), bz * 0.3f) * (amplitude);
			elevation += (noise.noise(bx * 0.6f, bz * 0.6f) * amplitude / 5);
			elevation -= (noise.noise((bx * 0.05f), (bz * 0.05f)) * (amplitude / 2));

			float deviation = noise.noise((bx * 8), (bz * 8)) * (amplitudeExtreme / 16);
			if (deviation > 0.95f)
				elevation += deviation;
			deviation = noise.noise((bx / 16), (bz / 16)) * (amplitudeExtreme);
			if (deviation > 0.95f)
				elevation += deviation;
			elevation += 45;
			elevation = glm::floor(elevation);

			bool placeTree = (rand() % 100) == 0 ? true : false;


			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{

				if (y == elevation)
				{
					if (y < waterLevel)
						SetBlock({ x, y, z }, Sand);
					else
						SetBlock({ x, y, z }, GrassBlock);

				}
				if (y == elevation - 1)
				{
					if (y < waterLevel)
						SetBlock({ x, y, z }, Sand);
					else
						SetBlock({ x, y, z }, GrassBlock);
				}



				if (y < elevation)
					SetBlock({ x, y, z }, Dirt);


				if (y < elevation - 4)
					SetBlock({ x, y, z }, Stone);

				if (placeTree && y == elevation && y > waterLevel + 2)
				{
					if (x < 2 || x > 29 || z < 2 || z > 29)
						continue;

					for (int ix = x - 2; ix < x + 3; ix++)
					{
						for (int iy = y + 3; iy < y + 5; iy++)
						{
							for (int iz = z - 2; iz < z + 3; iz++)
								SetBlock({ ix, iy, iz }, Leaf);

						}
					}
					SetBlock({ x - 2, y + 4, z + 2 }, Air);
					SetBlock({ x - 2, y + 4, z - 2 }, Air);
					SetBlock({ x + 2, y + 4, z + 2 }, Air);
					SetBlock({ x + 2, y + 4, z - 2 }, Air);

					SetBlock({ x + 1, y + 5, z - 1 }, Leaf);
					SetBlock({ x - 1, y + 5, z - 1 }, Leaf);
					SetBlock({ x + 1, y + 5, z + 1 }, Leaf);
					SetBlock({ x - 1, y + 5, z + 1 }, Leaf);

					SetBlock({ x + 1, y + 5, z }, Leaf);
					SetBlock({ x, y + 5, z + 1 }, Leaf);
					SetBlock({ x - 1, y + 5, z }, Leaf);
					SetBlock({ x, y + 5, z - 1 }, Leaf);

					SetBlock({ x + 1, y + 6, z }, Leaf);
					SetBlock({ x, y + 6, z + 1 }, Leaf);
					SetBlock({ x - 1, y + 6, z }, Leaf);
					SetBlock({ x, y + 6, z - 1 }, Leaf);

					SetBlock({ x, y + 6, z }, Leaf);
					SetBlock({ x, y + 5, z }, Leaf);


					SetBlock({ x, y + 1, z }, TreeLog);
					SetBlock({ x, y + 2, z }, TreeLog);
					SetBlock({ x, y + 3, z }, TreeLog);
					SetBlock({ x, y + 4, z }, TreeLog);
				}

				blockWereOn++;
			}
		}
	}

	for (int x = 0; x < CHUNK_WIDTH; x++)
	{
		for (int z = 0; z < CHUNK_DEPTH; z++)
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				if (y < 50)
				{
					if (GetBlock({ x, y, z }) == Air)
					{
						SetBlock({ x, y, z }, Water);
					}
				}
				if (y > 50)
					continue;
			}
		}
	}
	m_ReadyToGenerateMesh = true;
}

void Chunk::Generate(const SimplexNoise& noise)
{
	if (!m_HasChanged)
		return;

	m_ChunkBuilderThread = std::thread(&Chunk::GenerateBlocks, this, std::ref(noise));
	
}

