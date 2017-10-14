#pragma once
//Standard Library
#include <vector>

//Local headers
#include "Types.h"
#include "Block.h"

struct Chunk {
	//Local position of the chunk in the world. Equivalent to the position of the first block of the chunk
	//At the bottom, on top left.
	float32 xPos;
	float32 yPos;
	float32 zPos;

	//OpenGL Buffers
	uint32 VAO;					//VAO handle for all the VBOs (blocks) in the chunk
	std::vector<uint32> VBOs; 	//Vector of all block data
};