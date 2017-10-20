#include "TerrainBuilder.h"


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
	vector<Block> chunkBlocks= getChunkHeightmap(chunk);
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

	for (int row = 0; row < chunkSize; row++)
	{
		for (int col = 0; col < chunkSize; col++)
		{
			// Get Block position 
			float xPos = chunkOriginPos.x + col;
			float zPos = chunkOriginPos.z + row;
			float yPos = (noiseGenerator.GetNoise(xPos, zPos)+1)*.5; // height range:[-1,1]->[0,1] (Normalize height to between 0 and 1)
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

			// Check left block if not first column
			if(col > 0)
				fillVerticalGaps(chunkBlocks[index], chunkBlocks[index - 1], chunkBlocks);
			else
			{
				// Check if block on other chunk to the left is much lower than curr block
				glm::vec3 leftBlockPos = chunkBlocks[index].getPosition() - glm::vec3(1, 0, 0); 
				fillVerticalGaps(chunkBlocks[index], leftBlockPos, chunkBlocks);
			}

			// Check up block if not first row
			if (row > 0)
				fillVerticalGaps(chunkBlocks[index], chunkBlocks[index - chunkWidth], chunkBlocks);
			else
			{
				// Check if block on other chunk further in +z direction is much lower than curr block
				glm::vec3 upBlockPos = chunkBlocks[index].getPosition() + glm::vec3(0, 0, 1);
				fillVerticalGaps(chunkBlocks[index], upBlockPos, chunkBlocks);
			}

			// Check right block on next chunk if currBlock on last column
			if(col == chunkWidth - 1) {
				// Check if block on other chunk to the right is much lower than curr block
				glm::vec3 rightBlockPos = chunkBlocks[index].getPosition() + glm::vec3(1, 0, 0);
				fillVerticalGaps(chunkBlocks[index], rightBlockPos, chunkBlocks);
			}

			// Check down block on next chunk if currBlock on last row
			if (row == chunkWidth - 1) {
				// Check if block on other chunk further in -z direction is much lower than curr block
				glm::vec3 downBlockPos = chunkBlocks[index].getPosition() - glm::vec3(0, 0, 1);
				fillVerticalGaps(chunkBlocks[index], downBlockPos, chunkBlocks);
			}

		}
	}
}

void TerrainBuilder::fillVerticalGaps(Block& currBlock, Block& checkBlock, vector<Block>& chunkBlocks)
{
	int heightDifference = currBlock.getPosition().y - checkBlock.getPosition().y;
	
	// If currBlock is two or more block heights higher than the checked Block, duplicate currBlock
	if(heightDifference > 1)
		duplicateBlockVertically(currBlock, heightDifference, chunkBlocks);

	// If Checked Block is two or more block heights higher than the curr Block, duplicate Checked Block
	if (heightDifference < -1)
		duplicateBlockVertically(checkBlock, abs(heightDifference), chunkBlocks);
}

void TerrainBuilder::fillVerticalGaps(Block& currBlock, glm::vec3& checkPosition, vector<Block>& chunkBlocks)
{
	// Get height value of checked block in other chunk
	float yPos = (noiseGenerator.GetNoise(checkPosition.x, checkPosition.z) + 1)*.5; // height range:[-1,1]->[0,1] (Normalize height to between 0 and 1)
	yPos = floor(yPos*ChunkManager::CHUNKHEIGHT); // Make the y-position be a discrete value bw 0 and max chunk height

	int heightDifference = currBlock.getPosition().y - yPos;

	// If currBlock is two or more block heights higher than the checked Block, duplicate currBlock
	if (heightDifference > 1)
		duplicateBlockVertically(currBlock, heightDifference, chunkBlocks);

}

void TerrainBuilder::duplicateBlockVertically(Block& BlockToDuplicate, int heightDifference, vector<Block>& chunkBlocks)
{
	for (int i = 1; i < heightDifference; i++)
	{
		glm::vec3 newBlockPos = BlockToDuplicate.getPosition() - glm::vec3(0, i, 0);
		Block newBlock(newBlockPos, getBlockType(newBlockPos.y));
		chunkBlocks.push_back(newBlock);
	}
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
