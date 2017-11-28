#include "ShadowMap.h"

#include "GL\glew.h"

#include "glm/gtc/matrix_transform.hpp"

#include "RenderingContext.h"
#include "ChunkManager.h"
#include "ShaderProgram.h"

#include <iostream>

static const float32 sunDirectionMult = 300.0f;

ShadowMap::ShadowMap(uint32 width, uint32 height, glm::vec3 lightDirection)
	:m_width(width)
	,m_height(height)
{
	m_shadowShader = RenderingContext::get()->shaderCache.loadShaderProgram("sm_shader", "shadowmap_vert.glsl", "shadowmap_frag.glsl");
	updateMVP(lightDirection);
	buildFBO();
}

void ShadowMap::updateMVP(const glm::vec3& lightDirection)
{
	//The direction of the light inverted to go torwards the sky instead. Multiplied by a variable for distance.
	glm::vec3 invLightDirection = glm::normalize(-lightDirection) * sunDirectionMult;
	glm::vec3 playerPos = ChunkManager::instance()->getCurrentChunk(RenderingContext::get()->camera.transform.getPosition());

	//Setting the x and z components of the light position to follow the player throughout the terrain. The y value is constant.
	glm::vec3 lightPosition = { playerPos.x + invLightDirection.x, invLightDirection.y, playerPos.z + invLightDirection.z };

	//The radius we need for the orthographic matrix to include all the terrain based on the current chunk loading radius.
	float32 loadingRadius = (ChunkManager::LOADINGRADIUS + 0.5) * ChunkManager::CHUNKWIDTH;

	//Orthographic projection matrix from the point of view of the sun. The +50 value is just to be safe and include a little bit more of the terrain than needed.
	glm::mat4 proj = glm::ortho<float>(-loadingRadius, loadingRadius, 0, ChunkManager::CHUNKHEIGHT, 0.1f , 500.0f);

	//Lookat matrix from the light position to look at the x and z component of the player but at 0 in the y axis.
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

void ShadowMap::bindForWriting()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
}

void ShadowMap::bindForReading()
{
	glActiveTexture(Texture::UNIT_1);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
}

ShadowMap::~ShadowMap()
{
	glDeleteFramebuffers(1, &m_shadowFBO);
	glDeleteTextures(1, &m_depthTexture);
}
