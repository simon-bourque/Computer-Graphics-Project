#include "ShadowMap.h"

#include "GL\glew.h"

#include "glm/gtc/matrix_transform.hpp"

#include "RenderingContext.h"
#include "ChunkManager.h"

#include <iostream>

ShadowMap::ShadowMap(uint32 width, uint32 height, glm::vec3 lightDirection)
	:m_width(width)
	,m_height(height)
{
	updateMvp(lightDirection);
	buildBuffer();
}

void ShadowMap::updateMvp(glm::vec3 lightDirection)
{
	glm::vec3 invLightDirection = glm::normalize(-lightDirection);
	glm::vec3 playerPos = RenderingContext::get()->camera.transform.getPosition();
	glm::vec3 lightPosition = playerPos + (invLightDirection*200.0f);
	lightPosition.y = 100.0f;
	float32 loadingRadius = ChunkManager::instance()->getLoadingRadius() * ChunkManager::CHUNKWIDTH;

	glm::mat4 proj = glm::ortho(-loadingRadius, loadingRadius, -256.0f, 256.0f, 0.0f, 500.0f);
	glm::mat4 view = glm::lookAt(lightPosition, glm::vec3(playerPos.x,0,playerPos.z), glm::vec3(0, 1, 0));
	glm::mat4 mod = glm::mat4(1.0f);
	
	m_MVP = proj * view * mod;
}

void ShadowMap::updateSize(int32 width, int32 height)
{
	m_width = width;
	m_height = height;
	glDeleteFramebuffers(1, &m_fbo);
	glDeleteTextures(1, &m_texture);
	buildBuffer();
}

void ShadowMap::buildBuffer()
{
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
ShadowMap::~ShadowMap()
{
}
