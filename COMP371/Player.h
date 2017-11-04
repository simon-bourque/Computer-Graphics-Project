#pragma once

#include <vector>

#include "Entity.h"

class Player : public Entity
{
public:
	Player();
	virtual ~Player();

	glm::vec3 getPosition() { return m_position; };

	void update(float32 deltaSeconds);

private:
	void checkChunk();
	void checkForSurroundingBlocks();

	glm::vec3 m_position;
	glm::vec3 m_chunkPosition;
	std::vector<glm::vec3> m_chunkPositions;
};
