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
private:
	FastNoise noiseGenerator;
	FastNoise TreeNoiseGenerator;
	BlockType getBlockType(const float elevation);
	vector<Block> getChunkHeightmap(Chunk chunk);

	void fillChunkGaps(vector<Block>& chunkBlocks);
	void duplicateBlockVertically(Block BlockToDuplicate, int heightDifference, vector<Block>& chunkBlocks);
	glm::vec3 getHeightmapPosition(glm::vec3 xzPosition);


	// Tree Functions
	void makeTrunk(Block rootBlock, int trunkWidth, int trunkHeight, vector<Block>& chunkBlocks);
	void makeBlock(glm::vec3 rootBlock, int blockWidth, int blockHeight, vector<Block>& chunkBlocks, bool centeredAroundRoot = false);
	void makeUglyTree(Block rootBlock, int trunkHeight, vector<Block>& chunkBlocks);
	void makeUglyChristmasTree(Block rootBlock, int trunkHeight, vector<Block>& chunkBlocks);

	int closestPrimes[101];
	bool isPrime(int n);
	void fillClosestPrimes(int* closestPrimes);
};

