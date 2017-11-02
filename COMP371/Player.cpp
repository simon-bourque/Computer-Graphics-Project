#include "Player.h"

#include <iostream>

Player::Player()
{
	m_position = glm::vec3(0.0f);
}
Player::~Player() {}

void Player::update(float32 deltaSeconds)
{
	m_position = glm::vec3(transform->xPos, transform->yPos, transform->zPos);
	std::cout << "Player at (" << m_position.x << "," << m_position.y << "," << m_position.z << ")" << std::endl;
}
