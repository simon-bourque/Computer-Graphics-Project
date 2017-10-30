#include "LightSource.h"

// Light caster constructor
LightSource::LightSource(glm::vec3 direction, glm::vec3 color, float32 ambStrength, float32 specStrength)
	: m_color(color)
	, m_ambStrength(ambStrength)
	, m_specStrength(specStrength)
{
	m_direction = glm::normalize(-direction);
}

// Point light constructor
LightSource::LightSource(glm::vec3 position, glm::vec3 direction, glm::vec3 color, float32 ambStrength, float32 specStrength)
	: m_color(color)
	, m_position(position)
	, m_ambStrength(ambStrength)
	, m_specStrength(specStrength)
{
	m_direction = glm::normalize(-direction);
}


LightSource::~LightSource()
{
}
