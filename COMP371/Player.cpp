#include <iostream>

#include "Player.h"
#include "ChunkManager.h"
#include "AABB.h"
#include "InputManager.h"

Player::Player()
	: m_position(glm::vec3(0.0f))
	, m_forwardPressed(false)
	, m_backwardPressed(false)
	, m_leftPressed(false)
	, m_rightPressed(false)
	, m_upPressed(false)
	, m_downPressed(false)
{
	InputManager::instance()->registerKeyCallback(std::bind(&Player::onKey, this, std::placeholders::_1, std::placeholders::_2));
}

Player::~Player() {}

void Player::update(float32 deltaSeconds)
{
	const static float32 SPEED = 100.0f;
	const static float32 ROTATE_SPEED = 0.01f;

	// last position;
	m_position = glm::vec3(transform.xPos, transform.yPos, transform.zPos);

	float32 dx = 0;
	float32 dy = 0;
	float32 dz = 0;

	if (m_forwardPressed) {
		--dz;
	}
	if (m_backwardPressed) {
		++dz;
	}
	if (m_leftPressed) {
		--dx;
	}
	if (m_rightPressed) {
		++dx;
	}
	if (m_upPressed) {
		++dy;
	}
	if (m_downPressed) {
		--dy;
	}

	glm::vec3 deltaPos(dx, dy, dz);
	deltaPos = (glm::normalize(deltaPos) * SPEED * deltaSeconds);

	if (dx != 0 || dy != 0 || dz != 0)
	{
		transform.translateLocal(deltaPos.x, deltaPos.y, deltaPos.z);
	}

	//std::cout << "Player at (" << m_position.x << "," << m_position.y << "," << m_position.z << ")" << std::endl;

	//Check if the chunk you are in has changed
	checkChunk();

	//Bruteforce check for collision
	bool coll = checkForSurroundingBlocks();

	if (coll)
	{
		std::cout << "Colliding" << std::endl;
		transform.xPos = m_last_valid_global_position.x;
		transform.yPos = m_last_valid_global_position.y;
		transform.zPos = m_last_valid_global_position.z;

	}
	else
	{
		m_last_valid_global_position = m_position;
	}
}

void Player::checkChunk()
{
	glm::vec3 currentChunkPosition = ChunkManager::instance()->getCurrentChunk(m_position);

	if (currentChunkPosition != m_chunkPosition)
	{
		Chunk c = ChunkManager::instance()->getChunkHandle(currentChunkPosition);
		m_chunkPosition = currentChunkPosition;
		m_chunkPositions = c.getBlockPositions();
		//std::cout << "currentChunk at (" << currentChunkPosition.x << "," << currentChunkPosition.y << "," << currentChunkPosition.z << ")" << std::endl;
	}
}

bool Player::checkForSurroundingBlocks()
{
	AABB me = AABB::centeredOnPoint(m_position, 1);
	if (m_chunkPositions.size() > 0)
	{
		for (auto& it : m_chunkPositions)
		{
			AABB other = AABB(it, 2);
			if(AABB::checkCollision(me, other))
				return true;
		}
	}
	return false;
}

void Player::onKey(int32 key, int32 action)
{
	if (key == GLFW_KEY_W) {
		if (action == GLFW_PRESS) {
			m_forwardPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_forwardPressed = false;
		}
	}
	if (key == GLFW_KEY_S) {
		if (action == GLFW_PRESS) {
			m_backwardPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_backwardPressed = false;
		}
	}
	if (key == GLFW_KEY_A) {
		if (action == GLFW_PRESS) {
			m_leftPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_leftPressed = false;
		}
	}
	if (key == GLFW_KEY_D) {
		if (action == GLFW_PRESS) {
			m_rightPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_rightPressed = false;
		}
	}
	if (key == GLFW_KEY_SPACE) {
		if (action == GLFW_PRESS) {
			m_upPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_upPressed = false;
		}
	}
	if (key == GLFW_KEY_LEFT_CONTROL) {
		if (action == GLFW_PRESS) {
			m_downPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_downPressed = false;
		}
	}
}
