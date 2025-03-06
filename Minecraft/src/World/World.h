#pragma once

#include <vector>
#include <iostream>

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

struct AABB {
	glm::vec3 min;  // Bottom-left corner (x, y, z)
	glm::vec3 max;  // Top-right corner (x, y, z)

	AABB(const glm::vec3& position, const glm::vec3& size) {
		//min = glm::min(position, position + size);
		//max = glm::max(position, position + size);

		glm::vec3 end = position + size;
		min = glm::vec3(
			std::min(position.x, end.x),
			std::min(position.y, end.y),
			std::min(position.z, end.z)
		);
		max = glm::vec3(
			std::max(position.x, end.x),
			std::max(position.y, end.y),
			std::max(position.z, end.z)
		);

	}

	bool Intersects(const AABB& other) const {
		return 
			(min.x <= other.max.x && max.x >= other.min.x) &&
			(min.y <= other.max.y && max.y >= other.min.y) &&
			(min.z <= other.max.z && max.z >= other.min.z);
	}
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

	Block GetBlock(const glm::ivec3& position);
	void SetBlock(const glm::ivec3& position, BlockType block);

	inline AABB getBlockAABB(const glm::ivec3& pos) const {
		return { 
			{ pos.x, pos.y, pos.z }, 
			{ pos.x + 1, pos.y + 1, pos.z + 1 } 
		};
	}

	bool CheckCollision(const AABB& boundingBox) {

		// Use integer casting directly to avoid precision errors
		int startX = (int)boundingBox.min.x > 0 ? boundingBox.min.x : boundingBox.min.x - 1;
		int startY = (int)boundingBox.min.y > 0 ? boundingBox.min.y : boundingBox.min.y - 1;
		int startZ = (int)boundingBox.min.z > 0 ? boundingBox.min.z : boundingBox.min.z - 1;

		// Ensure max boundaries don't go out of bounds
		int endX = (int)boundingBox.max.x < 0 ? boundingBox.max.x : boundingBox.max.x + 0;
		int endY = (int)boundingBox.max.y < 0 ? boundingBox.max.y : boundingBox.max.y + 0;
		int endZ = (int)boundingBox.max.z < 0 ? boundingBox.max.z : boundingBox.max.z + 0;

		std::cout << "Checking collision from: (" << startX << ", " << startY << ", " << startZ << ")"
			<< " to (" << endX << ", " << endY << ", " << endZ << ")\n";

		for (int x = (int)boundingBox.min.x; x <= (int)boundingBox.max.x; ++x) {
			for (int y = (int)boundingBox.min.y; y <= (int)boundingBox.max.y; ++y) {
				for (int z = (int)boundingBox.min.z; z <= (int)boundingBox.max.z; ++z) {
					if (GetBlock({ x, y, z }).IsSolid() && boundingBox.Intersects(getBlockAABB({ x, y, z }))) {
						return true;  // Collision detected
					}
				}
			}
		}
		return false;  // No collision
	}

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
