#pragma once

#include "Entity.h"

class Player : public Entity
{
public:
	Player();
	virtual ~Player();

	glm::vec3 getPosition() { return m_position; };

	void update(float32 deltaSeconds);

private:
	glm::vec3 m_position;
};
