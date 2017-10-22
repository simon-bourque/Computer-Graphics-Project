#include "TerrainBuilder.h"
#include "ChunkManager.h"

TerrainBuilder::TerrainBuilder(int seed)
{
	noiseGenerator.SetSeed(seed);
	noiseGenerator.SetNoiseType(FastNoise::PerlinFractal);
}

TerrainBuilder::~TerrainBuilder()
{
}

/**
 * Returns vector with all blocks of given chunk. 
 * First (width*width) blocks represent Chunk heightmap (highest block for any given x-z coordinate)
 */
vector<Block> TerrainBuilder::getChunkBlocks(Chunk chunk)
{
	vector<Block> chunkBlocks = getChunkHeightmap(chunk);
	fillChunkGaps(chunkBlocks);
	return chunkBlocks;
}

/**
 * Returns a heightmap with a "grid" of blocks with 1 block per x-z position in the chunk
 */
vector<Block> TerrainBuilder::getChunkHeightmap(Chunk chunk)
{
	// Get chunk origin position instead of center position
	int chunkSize = ChunkManager::CHUNKWIDTH;
	glm::vec3 chunkOriginPos = chunk.getPosition() - glm::vec3(chunkSize/2.0f,0, chunkSize / 2.0f); // Assumes chunk y-pos = 0
	
	vector<Block> heightmapBlocks;
	heightmapBlocks.reserve(chunkSize * chunkSize);
	bool flipper = true;
	for (int row = 0; row < chunkSize; row++)
	{
		for (int col = 0; col < chunkSize; col++)
		{
			// Get Block position 
			float xPos = chunkOriginPos.x + col;
			float zPos = chunkOriginPos.z + row;
			float yPos = (noiseGenerator.GetNoise(xPos, zPos) + 1)*.5f; // height range:[-1,1]->[0,1] (Normalize height to between 0 and 1)
			yPos = floor(yPos*ChunkManager::CHUNKHEIGHT); // Make the y-position be a discrete value bw 0 and max chunk height

			glm::vec3 blockPos(xPos, yPos,zPos);

			// Create and push new heightMap block
			Block tempBlock(blockPos, getBlockType(blockPos.y));

			heightmapBlocks.push_back(tempBlock);
		}
	}
	return heightmapBlocks;
}

/**
 *  Modifies vector of blocks so that vertical gaps between adjacents blocks are filled by duplicating highest block down to fill gaps
 */
void TerrainBuilder::fillChunkGaps(vector<Block>& chunkBlocks)
{
	int chunkWidth = ChunkManager::CHUNKWIDTH;
	for (int row = 0; row < chunkWidth; row++)
	{
		for (int col = 0; col < chunkWidth; col++)
		{
			int index = row*chunkWidth + col;

			// Get positions of four adjacent blocks to current block
			glm::vec3 leftBlockPos, upBlockPos, downBlockPos, rightBlockPos;
			glm::vec3 currBlockPos = chunkBlocks[index].getPosition();


			// Get position of left adjacent block if not currently on  first column
			if (col > 0)
				leftBlockPos = chunkBlocks[index - 1].getPosition();
			else
				leftBlockPos = getHeightmapPosition( currBlockPos - glm::vec3(1, 0, 0) );

			// Get position of right adjacent block on next chunk if currBlock on last column
			if (col < chunkWidth - 1)
				rightBlockPos = chunkBlocks[index + 1].getPosition();
			else 
				rightBlockPos = getHeightmapPosition( currBlockPos + glm::vec3(1, 0, 0) );
			
			// Get position of up adjacent block if not currently on  first row
			if (row > 0)
				downBlockPos = chunkBlocks[index - chunkWidth].getPosition();
			else
				downBlockPos = getHeightmapPosition( currBlockPos - glm::vec3(0, 0, 1) );

			// Get position of down adjacent block if not currently on last row
			if (row < chunkWidth - 1)
				upBlockPos = chunkBlocks[index + chunkWidth].getPosition();
			else
				upBlockPos = getHeightmapPosition( currBlockPos + glm::vec3(0, 0, 1) );


			// Get maximum height difference bw current block and four adjacent blocks
			float heightDifference = currBlockPos.y - leftBlockPos.y;
			heightDifference = (heightDifference > currBlockPos.y - rightBlockPos.y) ? heightDifference : currBlockPos.y - rightBlockPos.y;
			heightDifference = (heightDifference > currBlockPos.y - upBlockPos.y) ? heightDifference : currBlockPos.y - upBlockPos.y;
			heightDifference = (heightDifference > currBlockPos.y - downBlockPos.y) ? heightDifference : currBlockPos.y - downBlockPos.y;

			// Duplicate the current block vertically to fill gaps
			if (heightDifference > 1.0f)
				duplicateBlockVertically(chunkBlocks[index], int(nearbyint(heightDifference)), chunkBlocks);

		}
	}
}

void TerrainBuilder::duplicateBlockVertically(Block BlockToDuplicate, int heightDifference, vector<Block>& chunkBlocks)
{
	for (int i = 1; i < heightDifference; i++)
	{
		glm::vec3 newBlockPos = BlockToDuplicate.getPosition() - glm::vec3(0, i, 0);
		Block newBlock(newBlockPos, getBlockType(newBlockPos.y));
		chunkBlocks.push_back(newBlock);
	}
}

glm::vec3 TerrainBuilder::getHeightmapPosition(glm::vec3 xzPosition)
{
	// Get height value at block position
	float yPos = (noiseGenerator.GetNoise(xzPosition.x, xzPosition.z) + 1)*.5f; // height range:[-1,1]->[0,1] (Normalize height to between 0 and 1)
	xzPosition.y = floor(yPos*ChunkManager::CHUNKHEIGHT); // Make the y-position be a discrete value bw 0 and max chunk height
	return xzPosition;
}

BlockType TerrainBuilder::getBlockType(const float elevation)
{
	int maxHeight = ChunkManager::CHUNKHEIGHT;
	if(elevation < 0.4f * maxHeight)	return BlockType::SAND; // Should be the elevation under which water shows up
	else if (elevation < 0.45f * maxHeight) return BlockType::DIRT;
	else if (elevation < 0.55f * maxHeight) return BlockType::GRASS;
	else if (elevation < 0.75f * maxHeight) return BlockType::DIRT; // Can later be changed to like mountain-type blocks
	else  return BlockType::SNOW;
}
