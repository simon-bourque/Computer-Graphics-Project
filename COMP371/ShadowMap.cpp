#include "ShadowMap.h"

#include "GL\glew.h"

#include "glm/gtc/matrix_transform.hpp"

#include "RenderingContext.h"
#include "ChunkManager.h"
#include "ShaderProgram.h"

#include <iostream>

ShadowMap::ShadowMap(uint32 width, uint32 height, glm::vec3 lightDirection)
	:m_width(width)
	,m_height(height)
{
	m_shadowShader = RenderingContext::get()->shaderCache.loadShaderProgram("sm_shader", "shadowmap_vert.glsl", "shadowmap_frag.glsl");
	updateMvp(lightDirection);
	buildFBO();
}

void ShadowMap::updateMvp(const glm::vec3& lightDirection)
{
	glm::vec3 invLightDirection = glm::normalize(-lightDirection) * 500.0f;
	glm::vec3 playerPos = RenderingContext::get()->camera.transform.getPosition();
	glm::vec3 lightPosition = { playerPos.x + invLightDirection.x, invLightDirection.y, playerPos.z + invLightDirection.z };
	float32 loadingRadius = (ChunkManager::LOADINGRADIUS + 0.5) * ChunkManager::CHUNKWIDTH;

	glm::mat4 proj = glm::ortho<float>(-loadingRadius, loadingRadius, 0, ChunkManager::CHUNKHEIGHT, 0.1f , 1000.0f);
	glm::mat4 view = glm::lookAt(lightPosition, glm::vec3(playerPos.x, 0, playerPos.z), glm::vec3(0, 1, 0));
	glm::mat4 mod = glm::mat4(1.0f);
	
	m_lightSpaceMVP = proj * view * mod;

	m_shadowShader->use();
	m_shadowShader->setUniform("lightSpaceMatrix", m_lightSpaceMVP);
}

void ShadowMap::updateSize(int32 width, int32 height)
{
	m_width = width;
	m_height = height;
	glDeleteFramebuffers(1, &m_shadowFBO);
	glDeleteTextures(1, &m_depthTexture);
	buildFBO();
}

void ShadowMap::buildFBO()
{
	glGenTextures(1, &m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glGenFramebuffers(1, &m_shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::bindTexture(Texture::Unit unit)
{
	glActiveTexture(unit);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
}

ShadowMap::~ShadowMap()
{
	glDeleteFramebuffers(1, &m_shadowFBO);
	glDeleteTextures(1, &m_depthTexture);
}
