#include "TerrainBuilder.h"
#include "ChunkManager.h"
#include "TreeBuilder.h"

TerrainBuilder::TerrainBuilder(int seed)
{
	noiseGenerator.SetSeed(seed);
	noiseGenerator.SetNoiseType(FastNoise::PerlinFractal);
	noiseGenerator.SetFrequency(0.005f);
	noiseGenerator.SetFractalOctaves(5);

	TreeNoiseGenerator.SetSeed(seed);
	TreeNoiseGenerator.SetNoiseType(FastNoise::SimplexFractal);
	TreeNoiseGenerator.SetFrequency(0.005f);
	TreeNoiseGenerator.SetFractalOctaves(2);

	fillClosestPrimes(closestPrimes);
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
 *  Also adds the trees to the terraub
 */
void TerrainBuilder::fillChunkGaps(vector<Block>& chunkBlocks)
{
	TreeBuilder treeGen(&chunkBlocks);

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

			// Load Tree
			if (chunkBlocks[index].getBlockType() == BlockType::GRASS)
			{
				// Trees are only placed if their positions are at prime numbers (number depends on noise)
				int noise = ((TreeNoiseGenerator.GetNoise(currBlockPos.x, currBlockPos.z) + 1) * 0.5f * 20) +5 ;
				if ((int)(currBlockPos.x+0) % closestPrimes[noise] == 0 && (int)(currBlockPos.z+0) % closestPrimes[noise] == 0 &&  currBlockPos.z != 0 && currBlockPos.x != 0)
					if ((int)leftBlockPos.y == (int)rightBlockPos.y && ((int)upBlockPos.y == (int)downBlockPos.y) && downBlockPos.y == (int)currBlockPos.y && rightBlockPos.y == (int)currBlockPos.y)
					{	
						// Depending on elevation make different kinds of trees
						if(currBlockPos.y < 0.6*ChunkManager::CHUNKHEIGHT)
							treeGen.makeTree(chunkBlocks[index].getPosition(),noise);
						else
							treeGen.makeChristmasTree(chunkBlocks[index].getPosition(),noise);
					}
			}

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

bool TerrainBuilder::isPrime(int n)
{
	if (n <= 1)
		return false;
	else if (n <= 3)
		return true;
	else if (n % 2 == 0 || n % 3 == 0)
		return false;
	int i = 5;
	while(i*i <= n)
	{
		if (n % i == 0 || n % (i + 2) == 0)
			return false;
		i += 6;
	}
	return true;
}

/**
 *  Fills an integer array where each index points to the closest prime larger than the index (up to prime 97)
 */
void TerrainBuilder::fillClosestPrimes(int* closestPrimes)
{

	int size = 101;
	closestPrimes[size -1] = 97;

	for(int i = size -2; i >= 0; i--)
	{
		if (isPrime(i))
			closestPrimes[i] = i;
		else
			closestPrimes[i] = closestPrimes[i+1];
	}
}

BlockType TerrainBuilder::getBlockType(const float elevation)
{
	int maxHeight = ChunkManager::CHUNKHEIGHT;
	if(elevation < 0.4f * maxHeight)	return BlockType::SAND; // Should be the elevation under which water shows up
	else if (elevation < 0.45f * maxHeight) return BlockType::DIRT;
	else if (elevation < 0.75f * maxHeight) return BlockType::GRASS;
	else  return BlockType::SNOW;
}
