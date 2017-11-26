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

	// Light-Space View matrix, light position is origin in light coord system
	glm::mat4 lightView = glm::lookAt(glm::vec3(0, 0, 0), glm::normalize(lightDirection), glm::vec3(0, 1, 0));

	// Get 8 corners camera view bounding frustum
	vector<glm::vec4> frustumCorners = RenderingContext::get()->camera.getFrustumCorners(45.0f, RenderingContext::get()->camera.getAspectRatio());

	// Get Camera View Matrix and Inverse
	glm::mat4 camView = RenderingContext::get()->camera.getViewMatrix();
	glm::mat4 camViewInverse = glm::inverse(camView);

	float minX = FLT_MAX;
	float maxX = -FLT_MAX;
	float minY = FLT_MAX;
	float maxY = -FLT_MAX;
	float minZ = FLT_MAX;
	float maxZ = -FLT_MAX;

	// Get the min/max values for the frustum's bounding box
	for (glm::vec4& corner : frustumCorners)
	{
		// Transform the frustum coordinate from view to world space
		// Then, transform the frustum coordinate from world to light space
		glm::vec4 worldSpace = camViewInverse * corner;
		corner = lightView * worldSpace;

		minX = min(minX, corner.x);
		maxX = max(maxX, corner.x);
		minY = min(minY, corner.y);
		maxY = max(maxY, corner.y);
		minZ = min(minZ, corner.z);
		maxZ = max(maxZ, corner.z);
	}

	//Orthographic projection matrix from the point of view of the sun. The +50 value is just to be safe and include a little bit more of the terrain than needed.
	glm::mat4 proj = glm::ortho<float>(minX, maxX, minY, maxY, minZ, maxZ);
	glm::mat4 mod = glm::mat4(1.0f);
	
	m_lightSpaceMVP = proj * lightView * mod;

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

	int32 mapSize = max(m_width, m_height);
	glGenTextures(1, &m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
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
