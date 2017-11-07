#pragma once
#include "Block.h"
#include <vector>
#include <functional>

using namespace std;

class TreeBuilder
{
public:
	TreeBuilder(vector<Block>* chunkBlockspt);
	~TreeBuilder();

	// Makes a Tree with a dome shaped crown (like an oak tree) and occasionally a cylindric crown
	void makeTree(glm::vec3 rootBlock, int noise);
	// Makes an evergreen tree (Christmas tree)
	void makeChristmasTree(glm::vec3 rootBlock, int noise);

private:
	vector<Block>* chunkBlocks;

	enum class Direction { LEFT, RIGHT, FORWARD, BACKWARD, UP, DOWN };
	glm::vec3 getDirection(Direction dir);

	void makeTrunk(glm::vec3 rootBlock, int trunkWidth, int trunkHeight);
	void makeBlock(glm::vec3 rootBlock, int blockWidth, int blockHeight, bool centeredAroundRoot = false);
	void makeSphere(glm::vec3 rootBlock, int radius, bool centeredAroundRoot = false);
	void makeBranch(glm::vec3 rootBlock, int length, int height, Direction dir, std::function<void(glm::vec3)> makeLeaves);

};

