#include <iostream>

#include "Player.h"
#include "ChunkManager.h"
#include "AABB.h"
#include "SphereCollider.h"
#include "InputManager.h"

glm::vec2 getMouseAxis();

Player::Player(Camera* camera)
	: m_camera(camera)
	, m_position(glm::vec3(0.0f))
	, m_forwardPressed(false)
	, m_backwardPressed(false)
	, m_leftPressed(false)
	, m_rightPressed(false)
	, m_jumpPressed(false)
	, m_rightMouseButtonPressed(false)
	, m_isJumping(false)
	, m_ready(false)
	, m_jumpedFrames(0)
	, m_collisionMode(CollisionMode::Sphere)
{
	InputManager::instance()->registerKeyCallback(std::bind(&Player::onKey, this, std::placeholders::_1, std::placeholders::_2));
	InputManager::instance()->registerMouseBtnCallback(std::bind(&Player::onMouseButton, this, std::placeholders::_1, std::placeholders::_2));
}

Player::~Player() {}

void Player::update(float32 deltaSeconds)
{
	const static float32 SPEED = 100.0f;
	const static float32 ROTATE_SPEED = 0.01f;
	const static int MAX_JUMP_FRAMES = 15;
	
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
	if (m_jumpPressed) {
		if (!m_isJumping)
			m_isJumping = true;
	}

	if(m_ready && !m_isJumping)
		dy += GRAVITY * deltaSeconds;
	else if (m_ready && m_isJumping)
	{
		dy -= GRAVITY * deltaSeconds;
		m_jumpedFrames++;

		if (m_jumpedFrames > MAX_JUMP_FRAMES)
		{
			m_jumpedFrames = 0;
			m_isJumping = false;
		}
	}

	glm::vec3 deltaPos(dx, dy, dz);
	deltaPos = (glm::normalize(deltaPos) * SPEED * deltaSeconds);
	deltaPos = glm::clamp(deltaPos, -0.9f, 0.9f);

	//Check if the chunk you are in has changed
	checkChunk();

	glm::vec3 newPosition = glm::vec3(transform.xPos + deltaPos.x, transform.yPos + deltaPos.y, transform.zPos + deltaPos.z);
	
	Collision coll = checkForSurroundingBlocks(newPosition, transform.yPos);

	switch (coll)
	{
	case(Collision::Colliding):
		std::cout << "Colliding" << std::endl;
		break;
	case(Collision::NoCollision):
		if (dx != 0 || dy != 0 || dz != 0)
			transform.translateLocal(deltaPos.x, deltaPos.y, deltaPos.z);
		break;
	case(Collision::CollidingNotY):
		if (dx != 0 || dz != 0)
			transform.translateLocal(deltaPos.x, 0.0f, deltaPos.z);
		break;
	case(Collision::NoCollisionUpOne):
		if (dx != 0 || dz != 0)
			transform.translateLocal(deltaPos.x, 1.0f, deltaPos.z);
		break;
	}

	m_camera->transform.xPos = transform.xPos;
	m_camera->transform.yPos = transform.yPos + 2.0f;
	m_camera->transform.zPos = transform.zPos;

	glm::vec2 mouseAxis = getMouseAxis() * ROTATE_SPEED;
	if (m_rightMouseButtonPressed) {

		if (abs(mouseAxis.x) > abs(mouseAxis.y)) {
			mouseAxis.y = 0;
		}
		else {
			mouseAxis.x = 0;
		}

		transform.rotate(0, -mouseAxis.x, 0);

		m_camera->transform.rotateLocal(mouseAxis.y, 0, 0);
		m_camera->transform.rotate(0, -mouseAxis.x, 0);
	}

	//std::cout << "Player at (" << m_position.x << "," << m_position.y << "," << m_position.z << ")" << std::endl;
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

Collision Player::checkForSurroundingBlocks(const glm::vec3& newPosition, const float32& currentY)
{
	glm::vec3 noY = glm::vec3(newPosition.x, currentY, newPosition.z);
	glm::vec3 upOne = glm::vec3(newPosition.x, currentY + 1.0f, newPosition.z);
	if (m_collisionMode == CollisionMode::AABB)
	{
		/*
		AABB me = AABB::centeredOnPoint(newPosition, 1.0f);
		AABB me2 = AABB::centeredOnPoint(noY, 1.0f);
		
		if (m_chunkPositions.size() > 0)
		{
			for (auto& it : m_chunkPositions)
			{
				AABB other = AABB(it, 1.0f);
				if (AABB::checkCollision(me, other))
				{
					if (AABB::checkCollision(me2, other))
						return Collision::Colliding;
					else
						return Collision::CollidingNotY;
				}
			}
		}
		*/
	}
	else
	{
		SphereCollider meAll = SphereCollider(newPosition, 1.0f);
		SphereCollider meNoY = SphereCollider(noY, 1.0f);
		SphereCollider meUpOne = SphereCollider(upOne, 1.0f);
		if (m_chunkPositions.size() > 0)
		{
			for (auto& it : m_chunkPositions)
			{
				SphereCollider other = SphereCollider::centeredOnVoxel(it);
				if (SphereCollider::checkCollision(meAll, other))
				{
					if (SphereCollider::checkCollision(meNoY, other))
					{
						if (SphereCollider::checkCollision(meUpOne, other))
							return Collision::Colliding;
						else
							return Collision::NoCollisionUpOne;
						
					}
					else
						return Collision::CollidingNotY;
				}
			}
		}
	}

	return Collision::NoCollision;
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
			m_jumpPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_jumpPressed = false;
		}
	}
	if (key == GLFW_KEY_ENTER) {
		if (action == GLFW_PRESS) {
			m_ready = true;
		}
	}
}
void Player::onMouseButton(int32 button, int32 action)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			m_rightMouseButtonPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_rightMouseButtonPressed = false;
		}
	}
}