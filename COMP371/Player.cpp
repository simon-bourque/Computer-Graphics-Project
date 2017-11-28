#include <iostream>

#include "Player.h"
#include "ChunkManager.h"
#include "AABBCollider.h"
#include "SphereCollider.h"
#include "InputManager.h"

#include <glm/gtx/projection.hpp>

glm::vec2 getMouseAxis();

Player::Player(Camera* camera)
	: m_camera(camera)
	, m_currentPosition(glm::vec3(0.0f))
	, m_forwardPressed(false)
	, m_backwardPressed(false)
	, m_leftPressed(false)
	, m_rightPressed(false)
	, m_jumpPressed(false)
	, m_rightMouseButtonPressed(false)
	, m_isJumping(false)
	, m_ready(false)
	, m_jumpedFrames(0)
	, m_collisionMode(CollisionMode::AABB)
	, m_chunkValid(false)
	, m_velocityY(0.0f)
	, m_glueToGround(false)
	, m_movedDuringFrame(false)
{
	InputManager::instance()->registerKeyCallback(std::bind(&Player::onKey, this, std::placeholders::_1, std::placeholders::_2));
	InputManager::instance()->registerMouseBtnCallback(std::bind(&Player::onMouseButton, this, std::placeholders::_1, std::placeholders::_2));
}

Player::~Player() {}

void Player::update(float32 deltaSeconds)
{
	const static float32 JUMP_POWER = 7.0f;
	const static float32 SPEED = 5.0f;
	const static float32 ROTATE_SPEED = 0.01f;
	const static int MAX_JUMP_FRAMES = 15;
	
	m_currentPosition = glm::vec3(transform.xPos, transform.yPos, transform.zPos);
	m_lastPosition = m_currentPosition;

	// Get current chunk
	ChunkManager* cm = ChunkManager::instance();
	m_chunkValid = cm->getChunkHandle(cm->getCurrentChunk(m_currentPosition), m_currentChunk);

	// DONT UPDATE ME IF MY CHUNK ISNT VALID YET GOOOSH
	if (!m_chunkValid) {
		return;
	}

	static bool hasLanded = false;

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
	glm::vec3 displacement = glm::rotate(transform.rotation, glm::vec3(dx, 0, dz));
	displacement.y = 0;
	displacement = glm::normalize(displacement);
	displacement *= SPEED * deltaSeconds;

	// Numerical approximation via Euler Integration
	dy = m_velocityY * deltaSeconds;
	m_velocityY += GRAVITY * deltaSeconds;

	//displacement.y = dy;

	//transform.translate(displacement);
	if (dx || dz) {
		transform.translate(displacement);
	}

	if (!m_glueToGround) {
		transform.translate(0, dy, 0);
	}

	if (dx || (dy && !m_glueToGround) || dz) {
		m_movedDuringFrame = true;
	}
	else {
		m_movedDuringFrame = false;
	}
	//transform.translateLocal(dx, 0, dz);
	//transform.translate(0, dy, 0);
	
	// Apply jump
	if (m_jumpPressed) {
		// Apply impulse and cancel gravity
		m_velocityY = JUMP_POWER;

		m_jumpPressed = false;
		m_glueToGround = false;
	}

	//if (m_jumpPressed) {
	//	if (!m_isJumping && hasLanded)
	//	{
	//		m_isJumping = true;
	//		hasLanded = false;
	//	}
	//}
	//
	//if(m_ready && !m_isJumping)
	//	dy += GRAVITY * deltaSeconds;
	//else if (m_ready && m_isJumping)
	//{
	//	dy -= GRAVITY * deltaSeconds;
	//	m_jumpedFrames++;
	//
	//	if (m_jumpedFrames > MAX_JUMP_FRAMES)
	//	{
	//		m_jumpedFrames = 0;
	//		m_isJumping = false;
	//	}
	//}

	glm::vec3 deltaPos(dx, dy, dz);
	deltaPos = (glm::normalize(deltaPos) * SPEED * deltaSeconds);

	// if you are in the water (below it), move half as much per frame
	if (m_currentPosition.y < m_swimY)
		deltaPos = glm::clamp(deltaPos, -0.45f, 0.45f);
	else
		deltaPos = glm::clamp(deltaPos, -0.9f, 0.9f);

	//Check if the chunk you are in has changed
	//checkChunk();
	//
	//glm::vec3 newPosition = glm::vec3(transform.xPos + deltaPos.x, transform.yPos + deltaPos.y, transform.zPos + deltaPos.z);
	//
	//Collision coll = checkForSurroundingBlocks(newPosition);
	//
	//switch (coll)
	//{
	//case(Collision::Colliding):
	//	std::cout << "Colliding" << std::endl;
	//	hasLanded = true;
	//	break;
	//case(Collision::NoCollision):
	//	if (dx != 0 || dy != 0 || dz != 0)
	//		transform.translateLocal(deltaPos.x, deltaPos.y, deltaPos.z);
	//	break;
	//case(Collision::CollidingNotY):
	//	if (dx != 0 || dz != 0)
	//		transform.translateLocal(deltaPos.x, 0.0f, deltaPos.z);
	//	hasLanded = true;
	//	break;
	//case(Collision::NoCollisionUpOne):
	//	if (dx != 0 || dz != 0)
	//		transform.translateLocal(deltaPos.x, 1.0f, deltaPos.z);
	//	hasLanded = true;
	//	break;
	//}

	/*
	Collision coll = checkForShubbery(newPosition);

	if (coll)
	{
		// congrats you hit a tree...
		switch (coll)
		{
		case(Collision::Colliding):
			hasLanded = true;
			break;
		case(Collision::CollidingXZ):
			if (dy != 0)
				transform.translateLocal(0.0f, deltaPos.y, 0.0f);
			break;
		}
	}
	else
	{
		coll = checkForSurroundingBlocks(newPosition);

		switch (coll)
		{
		case(Collision::Colliding):
			std::cout << "Colliding" << std::endl;
			hasLanded = true;
			break;
		case(Collision::NoCollision):
			if (dx != 0 || dy != 0 || dz != 0)
				transform.translateLocal(deltaPos.x, deltaPos.y, deltaPos.z);
			break;
		case(Collision::CollidingNotY):
			if (dx != 0 || dz != 0)
				transform.translateLocal(deltaPos.x, 0.0f, deltaPos.z);
			hasLanded = true;
			break;
		case(Collision::NoCollisionUpOne):
			if (dx != 0 || dz != 0)
				transform.translateLocal(deltaPos.x, 1.0f, deltaPos.z);
			hasLanded = true;
			break;
		}
	}
	*/

	// update the camera's transform
	m_camera->transform.xPos = transform.xPos;
	m_camera->transform.yPos = transform.yPos + 1.0f;
	m_camera->transform.zPos = transform.zPos;

	glm::vec2 mouseAxis = getMouseAxis() * ROTATE_SPEED;
	if (true) {

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

	m_currentPosition = glm::vec3(transform.xPos, transform.yPos, transform.zPos);

	if (m_movedDuringFrame) {
		processCollision();
	}
}

void Player::processCollision() {
	Manifold manifold;

	//std::cout << "################## COLLISION RESOLUTION ##################" << std::endl;
	while (checkChunkIntersection(manifold)) {
	//	//resolveChunkIntersection(manifold);
	}
	
	//bool collision = checkChunkIntersection(manifold);
	
	//if (collision) {
	//	resolveChunkIntersection(manifold);
	//}
}

bool Player::checkChunkIntersection(Manifold& manifold) {
	// Retrieve blocks for current chunk
	if (m_chunkValid) {
		const std::vector<glm::vec3>& blockPositions = m_currentChunk.getBlockPositions();

		// Create player collider
		glm::vec3 playerMinPoint(m_currentPosition.x - 0.5f, m_currentPosition.y - 1.0f, m_currentPosition.z - 0.5f);
		glm::vec3 playerMaxPoint(m_currentPosition.x + 0.5f, m_currentPosition.y + 1.0f, m_currentPosition.z + 0.5f);
		AABBCollider playerCollider(playerMinPoint, playerMaxPoint);

		for (const glm::vec3& block : blockPositions) {
			// Create AABB
			glm::vec3 minPoint(block.x - 0.5f, block.y - 0.5f, block.z - 0.5f);
			glm::vec3 maxPoint(block.x + 0.5f, block.y + 0.5f, block.z + 0.5f);
			AABBCollider collider(minPoint, maxPoint);

			// Check intersection
			if (AABBCollider::checkCollision(playerCollider, collider, manifold)) {
				//std::cout << "PIGNGNOFNOSFOSFNFON" << std::endl;
				// ###########################################################################################
				//glm::vec3 directionOfMovement(m_currentPosition - m_lastPosition);
				//std::cout << "(" << directionOfMovement.x << ", " << directionOfMovement.y << ", " << directionOfMovement.z << ")" << std::endl;
				//
				//glm::vec3 displacement-directionO;
				//
				//// If im moving in the positive x
				//if (directionOfMovement.x > 0) {
				//	
				//}
				//else if (directionOfMovement.x < 0) {
				//
				//}
				//else {
				//	displacement.x = 0;
				//}
				resolveChunkIntersection(manifold, collider);
				return true;
			}
		}
	}
	
	return false;
}

void Player::resolveChunkIntersection(const Manifold& manifold, const AABBCollider& blockCollider) {
	//transform.translate(manifold.seperationVector);
	
	//std::cout << "(" << manifold.seperationVector.x << ", " << manifold.seperationVector.y << ", " << manifold.seperationVector.z << ")" << std::endl;

	glm::vec3 directionOfMovement(m_currentPosition - m_lastPosition);
	//std::cout << "(" << directionOfMovement.x << ", " << directionOfMovement.y << ", " << directionOfMovement.z << ")" << std::endl;
	glm::vec3 seperation(manifold.seperationVector);

	// Add displacement to axes that caused the collision
	bool collideX = false;
	if (directionOfMovement.x != 0) {
		glm::vec3 playerMinPoint(m_lastPosition.x - 0.5f + directionOfMovement.x, m_lastPosition.y - 1.0f, m_lastPosition.z - 0.5f);
		glm::vec3 playerMaxPoint(m_lastPosition.x + 0.5f + directionOfMovement.x, m_lastPosition.y + 1.0f, m_lastPosition.z + 0.5f);
		AABBCollider playerCollider(playerMinPoint, playerMaxPoint);

		if (AABBCollider::checkCollision(playerCollider, blockCollider)) {
			collideX = true;
		}
	}

	bool collideY = false;
	if (directionOfMovement.y != 0) {
		glm::vec3 playerMinPoint(m_lastPosition.x - 0.5f, m_lastPosition.y - 1.0f + directionOfMovement.y, m_lastPosition.z - 0.5f);
		glm::vec3 playerMaxPoint(m_lastPosition.x + 0.5f, m_lastPosition.y + 1.0f + directionOfMovement.y, m_lastPosition.z + 0.5f);
		AABBCollider playerCollider(playerMinPoint, playerMaxPoint);

		if (AABBCollider::checkCollision(playerCollider, blockCollider)) {
			collideY = true;
		}
	}

	bool collideZ = false;
	if (directionOfMovement.z != 0) {
		glm::vec3 playerMinPoint(m_lastPosition.x - 0.5f, m_lastPosition.y - 1.0f, m_lastPosition.z - 0.5f + directionOfMovement.z);
		glm::vec3 playerMaxPoint(m_lastPosition.x + 0.5f, m_lastPosition.y + 1.0f, m_lastPosition.z + 0.5f + directionOfMovement.z);
		AABBCollider playerCollider(playerMinPoint, playerMaxPoint);

		if (AABBCollider::checkCollision(playerCollider, blockCollider)) {
			collideZ = true;
		}
	}


	//// Only displace in the axes the player is moving in
	//if (abs(directionOfMovement.x) == 0) {
	//	seperation.x = 0;
	//}
	//if (abs(directionOfMovement.y) == 0) {
	//	seperation.y = 0;
	//}
	//if (abs(directionOfMovement.z) == 0) {
	//	seperation.z = 0;
	//}


	// Only displace in the axes that caused the collision
	if (collideX || collideY || collideZ) {
		if (!collideX) {
			seperation.x = 0;
		}
		if (!collideY) {
			seperation.y = 0;
		}
		if (!collideZ) {
			seperation.z = 0;
		}
	}
	
	// Choose the minimum axis
	float32 depth = 0;
	int32 axis = 0;
	for (int32 i = 0; i < 3; i++) {
		if (depth == 0) {
			depth = seperation[i];
			axis = i;
		}
		else {
			if (seperation[i] != 0 && (abs(seperation[i]) < abs(depth))) {
				depth = seperation[i];
				axis = i;
			}
		}
	}
	for (int32 i = 0; i < 3; i++) {
		if (i == axis) {
			seperation[i] = depth;
		}
		else {
			seperation[i] = 0;
		}
	}

	//if (abs(seperation.x) < abs(seperation.y)) {
	//	if (abs(seperation.x) < abs(seperation.z)) {
	//		// min is x
	//		seperation.y = 0;
	//		seperation.z = 0;
	//	}
	//	else {
	//		// min is z
	//		seperation.y = 0;
	//		seperation.x = 0;
	//	}
	//}
	//else {
	//	if (abs(seperation.y) < abs(seperation.z)) {
	//		// min is y
	//		seperation.x = 0;
	//		seperation.z = 0;
	//	}
	//	else {
	//		// min is z
	//		seperation.y = 0;
	//		seperation.x = 0;
	//	}
	//}

	//glm::vec3 displacement = glm::proj(seperation, -glm::normalize(directionOfMovement));
	glm::vec3 displacement = seperation;
	std::cout << "(" << displacement.x << ", " << displacement.y << ", " << displacement.z << ")" << std::endl;
	
	// And just a hint of bias
	const static float32 bias = 0.0005f;
	if (abs(seperation.x) > 0) {
		displacement.x += signbit(seperation.x) ? -bias : bias;
	}
	if (abs(seperation.y) > 0) {
		displacement.y += signbit(seperation.y) ? -bias : bias;
	}
	if (abs(seperation.z) > 0) {
 		displacement.z += signbit(seperation.z) ? -bias : bias;
	}
	
	//m_lastPosition = m_currentPosition;
	transform.translate(displacement);
	m_currentPosition = glm::vec3(transform.xPos, transform.yPos, transform.zPos);

	if (displacement.y > 0) {
		// Stop!
		m_velocityY = 0;

		//m_glueToGround = true;
		
		// Just a hint of bias
		//transform.translate(0, 0.0005f, 0);
	}

	// If im moving in the positive x
	//if (directionOfMovement.x > 0) {
	//	
	//}
	//else if (directionOfMovement.x < 0) {
	//
	//}
	//else {
	//	displacement.x = 0;
	//}
	//
	//if (manifold.seperationVector.x == 0 && manifold.seperationVector.z == 0 && manifold.seperationVector.y > 0) {
	//	m_glueToGround = true;
	//
	//	// Stop!
	//	m_velocityY = 0;
	//	transform.translate(0, 0.0005f, 0); // HACK WARNING, Adding a bias here so I can get it to work for the demo
	//}
}

void Player::checkChunk()
{
	glm::vec3 currentChunkPosition = ChunkManager::instance()->getCurrentChunk(m_currentPosition);

	if (currentChunkPosition != m_chunkPosition)
	{
		Chunk c;
		bool gotChunk = ChunkManager::instance()->getChunkHandle(currentChunkPosition, c);

		if (gotChunk)
		{
			m_chunkPosition = currentChunkPosition;
			m_chunkPositions = c.getBlockPositions();
			m_chunkPositionsFoliage = c.getFoliageBlockPositions();

			// If gravity was not enabled, well then enable it
			if(!m_ready)
				m_ready = true;
		}
	}
}

Collision Player::checkForShubbery(const glm::vec3& newPosition)
{
	glm::vec3 noXZ = glm::vec3(m_currentPosition.x, newPosition.y, m_currentPosition.y);

	if (m_collisionMode == CollisionMode::AABB)
	{
		AABBCollider me = AABBCollider::centeredOnPoint(newPosition, 1.2f);
		AABBCollider me2 = AABBCollider::centeredOnPoint(noXZ, 1.2f);

		if (m_chunkPositionsFoliage.size() > 0)
		{
			for (auto& it : m_chunkPositionsFoliage)
			{
				AABBCollider other = AABBCollider::centeredOnPoint(it, 1.2f);
				if (AABBCollider::checkCollision(me, other))
				{
					if (AABBCollider::checkCollision(me2, other))
					{
						std::cout << "almost hittin' them good'ol foliage-xz" << std::endl;
						return Collision::CollidingXZ;
					}
					std::cout << "hittin' them good'ol foliage" << std::endl;
					return Collision::Colliding;
				}
			}
		}
	}
	else
	{
		SphereCollider meAll = SphereCollider(newPosition, 1.0f);
		SphereCollider meNoXZ = SphereCollider(noXZ, 1.0f);

		if (m_chunkPositionsFoliage.size() > 0)
		{
			for (auto& it : m_chunkPositionsFoliage)
			{
				SphereCollider other = SphereCollider(it, 1.0f);
				if (SphereCollider::checkCollision(meAll, other))
				{
					if (SphereCollider::checkCollision(meNoXZ, other))
					{
						std::cout << "almost hittin' them good'ol foliage-xz" << std::endl;
						return Collision::CollidingXZ;
					}
					std::cout << "hittin' them good'ol foliage" << std::endl;
					return Collision::Colliding;
				}
			}
		}
	}

	return Collision::NoCollision;
}

Collision Player::checkForSurroundingBlocks(const glm::vec3& newPosition)
{
	glm::vec3 noY = glm::vec3(newPosition.x, m_currentPosition.y, newPosition.z);
	glm::vec3 upOne = glm::vec3(newPosition.x, m_currentPosition.y + 1.0f, newPosition.z);
	if (m_collisionMode == CollisionMode::AABB)
	{
		AABBCollider me = AABBCollider::centeredOnPoint(newPosition, 1.2f);
		AABBCollider me2 = AABBCollider::centeredOnPoint(noY, 1.2f);
		AABBCollider me3 = AABBCollider::centeredOnPoint(upOne, 1.2f);

		if (m_chunkPositions.size() > 0)
		{
			for (auto& it : m_chunkPositions)
			{
				AABBCollider other = AABBCollider::centeredOnPoint(it, 1.2f);
				if (AABBCollider::checkCollision(me, other))
				{
					//ok you collided... is there a block above you?
					bool blockCrawl = false;
					for (auto& it2 : m_chunkPositions)
					{
						if (it2.x == it.x && it2.z == it.z && it2.y != it.y)
						{
							blockCrawl = true;
							break;// once is enough
						}
					}

					if (AABBCollider::checkCollision(me2, other))
					{
						if (AABBCollider::checkCollision(me3, other))
							return Collision::Colliding;
						else if(!blockCrawl)
							return Collision::NoCollisionUpOne;
						else
							return Collision::Colliding;
					}
					else
						return Collision::CollidingNotY;
				}
			}
		}
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
				SphereCollider other = SphereCollider(it, 1.0f);
				if (SphereCollider::checkCollision(meAll, other))
				{
					//ok you collided... is there a block above you?
					bool blockCrawl = false;
					for (auto& it2 : m_chunkPositions)
					{
						if (it2.x == it.x && it2.z == it.z && it2.y != it.y)
						{
							blockCrawl = true;
							break;// once is enough
						}
					}

					if (SphereCollider::checkCollision(meNoY, other))
					{
						if (SphereCollider::checkCollision(meUpOne, other))
							return Collision::Colliding;
						else if (!blockCrawl)
							return Collision::NoCollisionUpOne;
						else
							return Collision::Colliding;
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