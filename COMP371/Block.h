#pragma once

//Local headers
#include "Types.h"

enum class BlockType {
	GRASS, LOG, LEAVES, DIRT
};

struct Block {
	//Local position of the block in its chunk
	float32 xPos;
	float32 yPos;
	float32 zPos;

	BlockType blockType;
};