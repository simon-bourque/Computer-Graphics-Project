#include "ShadowMap.h"

#include "GL\glew.h"

#include "glm/gtc/matrix_transform.hpp"

#include "RenderingContext.h"

#include <iostream>

ShadowMap::ShadowMap(uint32 width, uint32 height, glm::vec3 lightDirection)
	:m_width(width)
	,m_height(height)
{
	updateMvp(lightDirection);
	initShaders();
	buildBuffer(width, height);
}

void ShadowMap::updateMvp(glm::vec3 lightDirection)
{
	glm::vec3 invLightDirection = glm::normalize(-lightDirection);
	glm::vec3 lightPosition = glm::vec3(50, 300, 0);
	glm::mat4 proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 1000.0f);
	glm::mat4 view = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0, 1, 0));

	glm::mat4 mod = glm::mat4(1.0f);
	m_MVP = proj * view * mod;
}

void ShadowMap::updateSize(int32 width, int32 height)
{
	glDeleteFramebuffers(1, &m_fbo);
	glDeleteTextures(1, &m_texture);
	buildBuffer(width, height);
}

void ShadowMap::buildBuffer(int32 width, int32 height)
{
	
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
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

void ShadowMap::initShaders()
{
	m_Shaders = RenderingContext::get()->shaderCache.loadShaderProgram("sm_shader", "shadowmap_vert.glsl", "shadowmap_frag.glsl");

	m_Shaders->use();
	m_Shaders->setUniform("depthMatrix", m_MVP);
}
ShadowMap::~ShadowMap()
{
}
