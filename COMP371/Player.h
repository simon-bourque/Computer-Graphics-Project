#pragma once

#include <vector>

#include "Camera.h"
#include "Entity.h"
#include "Collision.h"

class Player : public Entity
{
public:
	Player(Camera* camera);
	virtual ~Player();

	void update(float32 deltaSeconds);

	void setWaterHeight(const float& height) { m_swimY = height; };
	void setCollisionMode(const CollisionMode& mode) { m_collisionMode = mode; };

	glm::vec3 getPosition() { return m_position; };
	glm::vec3 getCurrentChunkPosition() const { return m_chunkPosition; };

private:
	void checkChunk();

	Collision checkForSurroundingBlocks(const glm::vec3& newPosition);
	Collision checkForShubbery(const glm::vec3& newPosition);

	void onKey(int32 key, int32 action);
	void onMouseButton(int32 button, int32 action);

	bool m_forwardPressed;
	bool m_backwardPressed;
	bool m_leftPressed;
	bool m_rightPressed;
	bool m_jumpPressed;

	bool m_rightMouseButtonPressed;

	bool m_ready;
	bool m_isJumping;
	int m_jumpedFrames;

	float m_swimY;

	CollisionMode m_collisionMode;

	glm::vec3 m_position;
	glm::vec3 m_chunkPosition;
	std::vector<glm::vec3> m_chunkPositions;
	std::vector<glm::vec3> m_chunkPositionsFoliage;

	Camera* m_camera;
};
