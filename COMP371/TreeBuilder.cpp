#include "TreeBuilder.h"

TreeBuilder::TreeBuilder(vector<Block>* chunkBlockspt)
{
	chunkBlocks = chunkBlockspt;
}

TreeBuilder::~TreeBuilder()
{
}

void TreeBuilder::makeTrunk(glm::vec3 rootBlock, int trunkWidth, int trunkHeight)
{
	// Make the Log
	for (int h = 1; h <= trunkHeight; h++)
		for (int c = -trunkWidth; c <= trunkWidth; c++)
			for (int r = -trunkWidth; r <= trunkWidth; r++)
			{
				// no use drawing the blocks inside the trunk since they're hidden
				if (r == -trunkWidth || r == trunkWidth || c == -trunkWidth || c == trunkWidth)
				{
					glm::vec3 newBlockPos = rootBlock + glm::vec3(c, h, r);
					Block newBlock(newBlockPos, BlockType::LOG);
					chunkBlocks->push_back(newBlock);
				}
			}
}

void TreeBuilder::makeBlock(glm::vec3 rootBlock, int blockWidth, int blockHeight, bool centeredAroundRoot)
{
	// Depending on whether the block is centered around the root or not, height range changes
	int h, maxH;
	if (!centeredAroundRoot)
	{
		h = 0;
		maxH = blockHeight;
	}
	else
	{
		h = -blockHeight / 2;
		maxH = blockHeight / 2;
	}

	// Make the Block
	for (h; h <= maxH; h++)
		for (int c = -blockWidth; c <= blockWidth; c++)
			for (int r = -blockWidth; r <= blockWidth; r++)
			{
				if (r == -blockWidth || r == blockWidth || c == -blockWidth || c == blockWidth || h == -blockHeight || h == blockHeight)
				{
					glm::vec3 newBlockPos = rootBlock + glm::vec3(c, h, r);
					Block newBlock(newBlockPos, BlockType::LEAVES);
					chunkBlocks->push_back(newBlock);
				}
			}
}

void TreeBuilder::makeSphere(glm::vec3 rootBlock, int radius, bool centeredAroundRoot)
{

	// Depending on whether the block is centered around the root or not, height range changes
	int h, maxH;
	if (!centeredAroundRoot)
	{
		h = 0;
		maxH = radius;
	}
	else
	{
		h = -radius / 2;
		maxH = radius / 2;
	}

	// Make the Block
	for (h; h <= maxH; h++)
		for (int c = -radius; c <= radius; c++)
			for (int r = -radius; r <= radius; r++)
			{
				glm::vec3 newBlockPos = rootBlock + glm::vec3(c, h, r);
				Block newBlock(newBlockPos, BlockType::LEAVES);

				// Block should only be rendered if it is inside the sphere volume around root Block
				float distFromOrigin = glm::distance(newBlockPos, rootBlock);
				if (distFromOrigin - 1 < radius && distFromOrigin + 1 < radius)
					chunkBlocks->push_back(newBlock);
			}
}

void TreeBuilder::makeBranch(glm::vec3 rootBlock, int length, int height, Direction dir, std::function<void(glm::vec3)> makeLeaves)
{

	glm::vec3 direction = getDirection(dir);
	glm::vec3 up = getDirection(Direction::UP);
	float h = 0;
	glm::vec3 newBlockPos = rootBlock;
	for (int i = 1; i <= length; i++)
	{
		// Make block that moves away from trunk
		newBlockPos += direction;
		Block newBlock(newBlockPos, BlockType::LOG);
		chunkBlocks->push_back(newBlock);

		// Make block that goes up one block height
		if(length - height < i)
		{
			h++;
			newBlockPos += up;
			Block newBlock(newBlockPos, BlockType::LOG);
			chunkBlocks->push_back(newBlock);
		}
	}
	
	// Make branch crown of leaves
	makeLeaves(newBlockPos);

}

glm::vec3 TreeBuilder::getDirection(Direction dir)
{
	switch(dir)
	{
		case Direction::LEFT: return glm::vec3(-1, 0, 0);
		case Direction::RIGHT: return glm::vec3(1, 0, 0);
		case Direction::UP: return glm::vec3(0, 1, 0);
		case Direction::DOWN:return glm::vec3(0, -1, 0);
		case Direction::FORWARD: return glm::vec3(0, 0, 1);
		case Direction::BACKWARD: return glm::vec3(0, 0, -1);
		default: return glm::vec3(0, 0, -1);
	}
}

void TreeBuilder::makeTree(glm::vec3 rootBlock, int noise)
{
	// Make the Log
	int trunkHeight = noise % 7 + 3;
	int trunksize = trunkHeight > 5 ? 1 : 0;
	makeTrunk(rootBlock, trunksize, trunkHeight);

	int crownRadius = 7 - (1 - trunksize) * 3;
	bool centeredAroundRoot = false;
	if (noise % 4 == 0) centeredAroundRoot = true; // 1 chance on 4 of having cylindric tree

	makeSphere(rootBlock + glm::vec3(0, trunkHeight, 0), crownRadius, centeredAroundRoot);

	// add branches if dome-shaped crowns
	if(trunksize == 1 && !centeredAroundRoot)
	{
		int branchHeight = 2 + trunkHeight %3;
		int branchLength = 3 + trunkHeight %4;
		auto makeLeaves = std::bind(&TreeBuilder::makeSphere, this, std::placeholders::_1, crownRadius / 2 + crownRadius%3, centeredAroundRoot);

		// get a 5-bit number to decide where to place branch(es)
		// If lowest bit is 0, no branches made (hence 50% chance of no branches on tree)
		// This ensures there's an equal chance for branches to grow on any side of the trunk
		int branchConfig = abs((int)(rootBlock.x + rootBlock.z)) % 32;
		if(branchConfig & 0x1 != 0)
		{
			if ((branchConfig & 0x2) != 0)
				makeBranch(rootBlock + glm::vec3(0, trunkHeight - branchHeight / 2 - 1, 0), branchLength, branchHeight, Direction::RIGHT, makeLeaves);

			if ((branchConfig & 0x4) != 0)
				makeBranch(rootBlock + glm::vec3(0, trunkHeight - branchHeight / 2 - 1, 0), branchLength, branchHeight, Direction::LEFT, makeLeaves);

			if ((branchConfig & 0x8) != 0)
				makeBranch(rootBlock + glm::vec3(0, trunkHeight - branchHeight / 2 - 1, 0), branchLength, branchHeight, Direction::FORWARD, makeLeaves);

			if ((branchConfig & 0x10) != 0)
				makeBranch(rootBlock + glm::vec3(0, trunkHeight - branchHeight / 2 - 1, 0), branchLength, branchHeight, Direction::BACKWARD, makeLeaves);
		}
	}

}

void TreeBuilder::makeChristmasTree(glm::vec3 rootBlock, int noise)
{
	// Make the Log
	int trunkHeight = noise % 3 + 4;
	int trunksize = trunkHeight > 5 ? 1 : 0;

	makeTrunk(rootBlock, trunksize, trunkHeight + 2);

	int leafBlockSize = trunkHeight;
	int lastLeafRad;

	for (int h = 0; h < leafBlockSize; h += 2)
	{
		// Every height level, make a smaller xz leaf plane
		int leafRad = ((leafBlockSize * 2 - h * 2 + h % 5) / 2);
		glm::vec3 originPos = rootBlock + glm::vec3(0, trunkHeight / 2 + 1, 0);
		lastLeafRad = leafRad;

		//  Make spaced out middle part (one layer leaves, one layer logs)
		for (int c = -leafRad; c <= leafRad; c++)
		{
			for (int r = -(leafRad - abs(c)); r <= (leafRad - abs(c)); r++)
			{
				glm::vec3 newBlockPos = originPos + glm::vec3(c, h, r);
				Block newBlock(newBlockPos, BlockType::LEAVES);
				chunkBlocks->push_back(newBlock);

				// Add pair of small '+' shaped logs every other layer
				if (r == 0 || c == 0)
				{
					newBlockPos = originPos + glm::vec3(c, h + 1, r);

					// if trunkHeight > 5 then you will have a large trunk with 3x3 layers
					if (r > -2 && c > -2 && r < 2 && c < 2 && trunkHeight >5) {
						Block newBlock2(newBlockPos, BlockType::LOG);
						chunkBlocks->push_back(newBlock2);
					}
				}

			}
		}

		// Add a pyramid top to these christmas trees
		for (int h = 0; h <= lastLeafRad; h++)
		{
			// Every height level, make a smaller xz leaf plane
			int leafRad = lastLeafRad - h - 2;
			glm::vec3 originPos = rootBlock + glm::vec3(0, trunkHeight / 2 + 1 + leafBlockSize, 0);

			//  Make spaced out middle part one layer leaves, one layer log
			for (int c = -leafRad; c <= leafRad; c++)
			{
				for (int r = -(leafRad - abs(c)); r <= (leafRad - abs(c)); r++)
				{

					glm::vec3 newBlockPos = originPos + glm::vec3(c, h, r);
					if (c == 0 && r == 0 && h < lastLeafRad - 2)
					{
						// Center Block should be a log
						Block newBlock(newBlockPos, BlockType::LOG);
						chunkBlocks->push_back(newBlock);
					}
					else
					{

						Block newBlock(newBlockPos, BlockType::LEAVES);
						chunkBlocks->push_back(newBlock);

					}

				}
			}
		}
	}
}
