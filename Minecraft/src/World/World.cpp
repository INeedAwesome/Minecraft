#include "World.h"
#include "Textures/TextureAtlas.h"


World::World()
{
	m_NoiseGenerator = SimplexNoise(0.1f, 0.5f, 1.99f, 0.5f);
}

World::~World()
{
	for (Chunk* chunk : m_Chunks)
		delete chunk;
}

void World::Update(const glm::vec3& playerPosition, bool deleteChunks)
{

	for (int i = 0; i < m_Chunks.size() && deleteChunks; i++)
	{
		int deltaX = m_Chunks[i]->GetPosition().x * CHUNK_WIDTH - (int)playerPosition.x;
		int deltaZ = m_Chunks[i]->GetPosition().z * CHUNK_DEPTH - (int)playerPosition.z;

		if (deltaX > CHUNK_WIDTH * m_ViewDistance || deltaX < -CHUNK_WIDTH * (m_ViewDistance + 1) || deltaZ > CHUNK_WIDTH * m_ViewDistance || deltaZ < -CHUNK_WIDTH * (m_ViewDistance + 1))
		{
			delete m_Chunks[i];
			m_Chunks.erase(m_Chunks.begin() + i);
			m_OccupiedChunks.erase(m_OccupiedChunks.begin() + i);
		}

	}

	for (Chunk* chunk : m_Chunks)
		chunk->Update();
}

void World::Render()
{
	TextureAtlas::Get().Bind();

	for (auto& chunk : m_Chunks)
	{
		chunk->Render();
	}

	TextureAtlas::Get().Unbind();
}

void World::SortChunksForRendering(const glm::vec3 playerPosition)
{
	m_SortedChunks.clear();
	for (unsigned int i = 0; i < m_Chunks.size(); i++)
	{
		float distance = glm::length(playerPosition - glm::vec3{m_Chunks[i]->GetPosition().x * CHUNK_DEPTH, playerPosition.y, m_Chunks[i]->GetPosition().z * CHUNK_DEPTH});
		m_SortedChunks[distance] = m_Chunks[i];
	}
}

void World::GenerateAroundPlayer(const glm::vec3 playerPosition)
{
	//return;
	glm::ivec3 playerChunkPos = { (int)playerPosition.x / CHUNK_WIDTH, 0, (int)playerPosition.z / CHUNK_WIDTH };

	for (int x = playerChunkPos.x - m_ViewDistance; x < m_ViewDistance + playerChunkPos.x; x++)
	{
		for (int z = playerChunkPos.z - m_ViewDistance; z < m_ViewDistance + playerChunkPos.z; z++)
		{
			GenerateAtPosition({x, z});
		}
	}
}

void World::GenerateAtPosition(const glm::vec2& pos)
{
	// if there isn't a chunk at pos.x and pos.z generate a new one

	for (int i = 0; i < m_OccupiedChunks.size(); i++)
	{
		if (pos == m_OccupiedChunks[i])
		{
			return;
		}
	}

	Chunk* chunk = new Chunk({ pos.x, 0, pos.y });
	chunk->Generate(m_NoiseGenerator);
	m_Chunks.push_back(chunk);
	m_OccupiedChunks.push_back(pos);
}

bool World::CastRay(const Ray& ray, glm::ivec3& hitBlock, glm::ivec3& hitNormal, float maxDistance) {
	glm::vec3 rayPos = ray.origin;

	float yawRad = glm::radians(ray.direction.x);
	float pitchRad = glm::radians(ray.direction.y);

	glm::vec3 direction;
	direction.x = cos(yawRad) * cos(pitchRad);
	direction.y = sin(pitchRad);
	direction.z = sin(yawRad) * cos(pitchRad);


	glm::vec3 rayDir = glm::normalize(direction);

	glm::ivec3 blockPos = glm::floor(rayPos);  // Get integer block coordinates
	glm::vec3 deltaDist = glm::vec3(
		fabs(1.0f / rayDir.x),
		fabs(1.0f / rayDir.y),
		fabs(1.0f / rayDir.z)
	);

	glm::ivec3 step(
		(rayDir.x > 0) ? 1 : -1,
		(rayDir.y > 0) ? 1 : -1,
		(rayDir.z > 0) ? 1 : -1
	);

	glm::vec3 sideDist(
		(rayDir.x > 0) ? (blockPos.x + 1 - rayPos.x) * deltaDist.x : (rayPos.x - blockPos.x) * deltaDist.x,
		(rayDir.y > 0) ? (blockPos.y + 1 - rayPos.y) * deltaDist.y : (rayPos.y - blockPos.y) * deltaDist.y,
		(rayDir.z > 0) ? (blockPos.z + 1 - rayPos.z) * deltaDist.z : (rayPos.z - blockPos.z) * deltaDist.z
	);


	// Step through the world grid
	for (float distance = 0.0f; distance < maxDistance && distance > -maxDistance; distance += 0.f) {
		// Check if the block exists in the world
		BlockType bt = GetBlock(blockPos);
		if (bt != Air) { 
			hitBlock = blockPos;
			return true;
		}

		// Move to next voxel
		if (sideDist.x < sideDist.y && sideDist.x < sideDist.z) {
			sideDist.x += deltaDist.x;
			blockPos.x += step.x;
			hitNormal = glm::ivec3(-step.x, 0, 0);
			distance += direction.x;
		}
		else if (sideDist.y < sideDist.z) {
			sideDist.y += deltaDist.y;
			blockPos.y += step.y;
			hitNormal = glm::ivec3(0, -step.y, 0);
			distance += direction.y;
		}
		else {
			sideDist.z += deltaDist.z;
			blockPos.z += step.z;
			hitNormal = glm::ivec3(0, 0, -step.z);
			distance += direction.z;
		}
	}

	return false;  // No block was hit
}

BlockType World::GetBlock(const glm::ivec3& position)
{
	int chunkX = (position.x < 0) ? (position.x - CHUNK_WIDTH + 1) / CHUNK_WIDTH : position.x / CHUNK_WIDTH;
	int chunkZ = (position.z < 0) ? (position.z - CHUNK_DEPTH + 1) / CHUNK_DEPTH : position.z / CHUNK_DEPTH;
	glm::vec3 chunkPos = { chunkX, 0, chunkZ };

	// Ensure local coordinates are positive
	int localX = ((int)position.x % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH;
	int localZ = ((int)position.z % CHUNK_DEPTH + CHUNK_DEPTH) % CHUNK_DEPTH;

	// Find the correct chunk
	for (Chunk* chunk : m_Chunks) {
		if (chunk->GetPosition().x == chunkPos.x && chunk->GetPosition().z == chunkPos.z) {
			return chunk->GetBlock({ localX, position.y, localZ });
		}
	}
	return Air;
}

void World::SetBlock(const glm::ivec3& position, BlockType block)
{
	int chunkX = (position.x < 0) ? (position.x - CHUNK_WIDTH + 1) / CHUNK_WIDTH : position.x / CHUNK_WIDTH;
	int chunkZ = (position.z < 0) ? (position.z - CHUNK_DEPTH + 1) / CHUNK_DEPTH : position.z / CHUNK_DEPTH;
	glm::vec3 chunkPos = { chunkX, 0, chunkZ };

	// Ensure local coordinates are positive
	int localX = ((int)position.x % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH;
	int localZ = ((int)position.z % CHUNK_DEPTH + CHUNK_DEPTH) % CHUNK_DEPTH;

	// Find the correct chunk
	for (Chunk* chunk : m_Chunks) {
		if (chunk->GetPosition().x == chunkPos.x && chunk->GetPosition().z == chunkPos.z) {
			chunk->SetBlock({ localX, position.y, localZ }, block);
			chunk->RegenerateChunkMesh();
			return;
		}
	}

}

