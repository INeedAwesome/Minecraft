#pragma once

#include <vector>

#include "Chunk.h"
#include "SimplexNoise.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <map>

struct Ray
{
	glm::vec3 origin;
	glm::vec3 direction;
};

class World
{
public:
	World();
	~World();

	void Update(const glm::vec3& playerPosition, bool deleteChunks);
	void Render();

	void SetViewDistance(int distance) { m_ViewDistance = distance; }
	void SortChunksForRendering(const glm::vec3 playerPosition);

	void GenerateAroundPlayer(const glm::vec3 playerPosition);

	bool CastRay(const Ray& ray, glm::ivec3& hitBlock, glm::ivec3& hitNormal, float maxDistance = 10.0f);

	BlockType GetBlock(const glm::vec3& position);
	void SetBlock(const glm::vec3& position, BlockType block);

private:
	void GenerateAtPosition(const glm::vec2& pos);

private:
	SimplexNoise m_NoiseGenerator;

	int m_ViewDistance = 5;

	std::vector<glm::vec2> m_OccupiedChunks;
	std::vector<Chunk*> m_Chunks;

	std::map<float, Chunk*> m_SortedChunks;


	// List of chunks that each has information like if it exists already... 
	//  - Maybe unordered_map ? 
	//  - Function IsChunkPosOccupied() ? 


};
