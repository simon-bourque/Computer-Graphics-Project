#pragma once

#include "Types.h"
#include "Texture.h"

#include "glm\glm.hpp"

class ShaderProgram;

class ShadowMap
{
public:
	ShadowMap(uint32 width, uint32 height, glm::vec3 lightDirection);

	void updateSize(int32 width, int32 height);
	void updateMvp(const glm::vec3& lightDirection);

	uint32 getFbo() const { return m_shadowFBO; }
	glm::mat4 getMVP() const { return m_lightSpaceMVP; }

	void bindTexture(Texture::Unit unit);

	~ShadowMap();
private:
	void buildFBO();

	int32 m_width;
	int32 m_height;

	glm::mat4 m_lightSpaceMVP;

	ShaderProgram* m_shadowShader;

	uint32 m_shadowFBO;
	uint32 m_depthTexture;
};

