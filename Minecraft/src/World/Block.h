#pragma once

#include <unordered_map>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#define BLOCK_TO_NAME(block) #block

enum BlockFace
{
	Front, Left, Right, Back, Bottom, Top
};

enum BlockType : unsigned char // 256 possible
{
	Air,
	Stone,
	Cobblestone,
	Dirt,
	GrassBlock,
	Water,
	Sand,
	TreeLog,
	Leaf,
	AMOUNT_BLOCKS
};

static std::unordered_map<BlockType, std::unordered_map<BlockFace, glm::vec2>> s_uv_xy = {
	{ Stone,      {{Front, {0.0f, 1.0f}}}},
	{ Cobblestone,{{Front, {2.0f, 0.0f}}}},
	{ Dirt,       {{Front, {1.0f, 0.0f}}}},

	{ GrassBlock, {
		{ Top,    {0.0f, 0.0f} },  // Grass top texture
		{ Bottom, {1.0f, 0.0f} },  // Dirt bottom texture
		{ Front,  {3.0f, 0.0f} },  // Grass side texture
		{ Back,   {3.0f, 0.0f} },
		{ Left,   {3.0f, 0.0f} },
		{ Right,  {3.0f, 0.0f} }
	}},
	{ Water,     {{Front, {4.0f, 0.0f}}}},
	{ Sand,      {{Front, {5.0f, 0.0f}}}},
	{ TreeLog,   {{Front, {6.0f, 0.0f}}}},
	{ Leaf,      {{Front, {7.0f, 0.0f}}}}

};

static glm::vec2 GetUV(BlockType type, BlockFace face) {
	if (type == AMOUNT_BLOCKS)
		throw std::invalid_argument("Type is of AMOUNT_BLOCKS");

	// Default to the front face if the face is not explicitly defined
	if (s_uv_xy[type].count(face)) {
		return s_uv_xy[type][face];
	}
	return s_uv_xy[type][Front];
}

class Block
{
public:
	BlockType blockType = Air;

	bool IsOpaque() const {
		if (blockType == Air || blockType == Water)
			return true;
		return false;
	}

	bool IsSolid() const
	{
		if (blockType == Air || blockType == Water)
			return false;
		return true;
	}
};

