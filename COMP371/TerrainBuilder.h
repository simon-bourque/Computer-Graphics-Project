#pragma once

#include "FastNoise/FastNoise.h"
#include "Chunk.h"
#include "ChunkManager.h"
#include "Block.h"

#include <vector>

using namespace std;

class TerrainBuilder
{
public:
	TerrainBuilder(int seed);
	~TerrainBuilder();
	vector<Block> getChunkBlocks(Chunk chunk);
private:
	FastNoise noiseGenerator;
	BlockType getBlockType(const float elevation);
	vector<Block> getChunkHeightmap(Chunk chunk);
	void fillChunkGaps(vector<Block>& chunkBlocks);

	void fillVerticalGaps(Block& currBlock, Block& checkBlock, vector<Block>& chunkBlocks);
	void fillVerticalGaps(Block& currBlock, glm::vec3& checkPosition, vector<Block>& chunkBlocks);
	void duplicateBlockVertically(Block& BlockToDuplicate, int heightDifference, vector<Block>& chunkBlocks);

};

