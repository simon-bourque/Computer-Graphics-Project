#pragma once

#include "FastNoise/FastNoise.h"
#include "Chunk.h"
#include "Block.h"

#include <vector>

using namespace std;

class TerrainBuilder
{
public:
	TerrainBuilder(int seed);
	~TerrainBuilder();
	vector<Block> getChunkBlocks(Chunk chunk);

	// In percentage of the top block elevation
	static float const MAX_WATER_ELEVATION;
	static const float  MAX_SAND_ELEVATION;
	static const float MAX_DIRT_ELEVATION;
	static const float MAX_GRASS_ELEVATION;

private:
	FastNoise noiseGenerator;
	FastNoise TreeNoiseGenerator;
	BlockType getBlockType(const float elevation);
	vector<Block> getChunkHeightmap(Chunk chunk);

	void fillChunkGaps(vector<Block>& chunkBlocks);
	void duplicateBlockVertically(Block BlockToDuplicate, int heightDifference, vector<Block>& chunkBlocks);
	glm::vec3 getHeightmapPosition(glm::vec3 xzPosition);

	// TODO see if you could use constexpr on this
	int closestPrimes[101];
	bool isPrime(int n);
	void fillClosestPrimes(int* closestPrimes);
};

