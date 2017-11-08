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
	LightSource(glm::vec3 position, glm::vec3 direction, glm::vec3 color, float32 ambientStrength, float32 specStrength);
	LightSource(glm::vec3 direction, glm::vec3 color, float32 ambientStrength, float32 specStrength);
	~LightSource();

	glm::vec3 getColor() { return m_color; }
	glm::vec3 getDirection() { return m_direction; }
	float32 getAmbStrength() { return m_ambStrength; }
	float32 getSpecStrength() { return m_specStrength; }

private:
	void initShaders();

	float32 m_ambStrength;
	float32 m_specStrength;

	SourceType m_type;

	glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec3 m_color;
};

