#pragma once

//Local headers
#include "Types.h"
#include "Block.h"

struct Chunk {
	//Local position of the chunk in the world
	float32 xPos;
	float32 yPos;
	float32 zPos;

	//OpenGL Buffers
	uint32 VAO;
};