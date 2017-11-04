#include <iostream>

#include "Player.h"
#include "ChunkManager.h"
#include "AABB.h"

Player::Player()
	: m_position(glm::vec3(0.0f))
{}

Player::~Player() {}

void Player::update(float32 deltaSeconds)
{
	m_position = glm::vec3(transform->xPos, transform->yPos, transform->zPos);
	//std::cout << "Player at (" << m_position.x << "," << m_position.y << "," << m_position.z << ")" << std::endl;

	//Check if the chunk you are in has changed
	checkChunk();

	//Bruteforce check for collision
	checkForSurroundingBlocks();
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

void Player::checkForSurroundingBlocks()
{
	//AABB me = AABB(m_position, 1);
	AABB me = AABB::centeredOnPoint(m_position, 1);
	if (m_chunkPositions.size() > 0)
	{
		for (auto& it : m_chunkPositions)
		{
			AABB other = AABB(it, 1);
			if(AABB::checkCollision(me, other))
			{
				std::cout << "Colliding" << std::endl;
			}
		}
	}
}
