#pragma once

#include "glm/glm.hpp"

#include "Types.h"

/**
 * A light source class meant to simulate light coming from the sun or the moon for example.
 */

enum class SourceType
{
	DIRECTIONAL, POINT
};

struct LightSource
{
public:
	LightSource(glm::vec3 position, glm::vec3 direction, glm::vec3 color);
	LightSource(glm::vec3 direction, glm::vec3 color);
	~LightSource();

	glm::vec3 getColor() const { return m_color; }
	glm::vec3 getDirection() const { return m_direction; }
private:
	void initShaders();

	SourceType m_type;

	glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec3 m_color;
};

