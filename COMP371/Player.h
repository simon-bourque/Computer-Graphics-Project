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
	bool checkForSurroundingBlocks();

	void onKey(int32 key, int32 action);
	bool m_forwardPressed;
	bool m_backwardPressed;
	bool m_leftPressed;
	bool m_rightPressed;
	bool m_upPressed;
	bool m_downPressed;

	glm::vec3 m_position;
	glm::vec3 m_last_valid_global_position;
	glm::vec3 m_chunkPosition;
	std::vector<glm::vec3> m_chunkPositions;
};
