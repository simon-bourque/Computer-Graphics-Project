#include "LightSource.h"

#include "RenderingContext.h"

// Light caster constructor
LightSource::LightSource(glm::vec3 direction, glm::vec3 color, float32 ambStrength, float32 specStrength)
	: m_color(color)
	, m_ambStrength(ambStrength)
	, m_specStrength(specStrength)
{
	m_type = SourceType::DIRECTIONAL;
	m_direction = glm::normalize(-direction);
	initShaders();
}

// Point light constructor
LightSource::LightSource(glm::vec3 position, glm::vec3 direction, glm::vec3 color, float32 ambStrength, float32 specStrength)
	: m_color(color)
	, m_position(position)
	, m_ambStrength(ambStrength)
	, m_specStrength(specStrength)
{
	m_type = SourceType::POINT;
	m_direction = glm::normalize(-direction);
	initShaders();
}

void LightSource::initShaders()
{
	ShaderProgram* chunkShader = RenderingContext::get()->shaderCache.getShaderProgram("chunk_shader");
	if (m_type == SourceType::POINT) { chunkShader->setUniform("lightPosition", m_position); }
	chunkShader->setUniform("lightColor", m_color);
	chunkShader->setUniform("lightDirection", m_direction);
	chunkShader->setUniform("ambientStrength", m_ambStrength);
	chunkShader->setUniform("specularStrength", m_specStrength);
}

LightSource::~LightSource()
{
}
