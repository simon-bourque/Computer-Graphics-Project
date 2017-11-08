#include "WaterRenderer.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "RenderingContext.h"
#include "ShaderProgram.h"
#include "Texture.h"

WaterRenderer* WaterRenderer::s_instance = nullptr;

WaterRenderer::WaterRenderer() : 
	m_y(100.0f),
	m_textureTileFactor(1.0f)
{
	m_waterShader = RenderingContext::get()->shaderCache.loadShaderProgram("water_shader", "water_vert.glsl", "water_frag.glsl");
	m_waterNormal = RenderingContext::get()->textureCache.loadTexture2D("water_normal", "water_normal.png");
	m_waterNormal2 = RenderingContext::get()->textureCache.loadTexture2D("water_normal2", "water_normal2.png");
	m_waterDuDv = RenderingContext::get()->textureCache.loadTexture2D("water_dudv", "water_dudv.png");

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	const static uint32 NUM_VERTICES = 4;
	const static float32 vertices[NUM_VERTICES * 3] = {
		-0.5f,0.0f,0.5f,
		-0.5f,0.0f,-0.5f,
		0.5f,0.0f,-0.5f,
		0.5,0.0f,0.5f
	};

	const static uint32 NUM_UV = 8;
	const static float32 uvs[] = {
		0.0f,0.0f,
		0.0f,1.0f,
		1.0f,1.0f,
		1.0f,0.0f
	};

	const static uint32 NUM_INDICES = 6;
	const static uint32 indices[NUM_INDICES] = {
		0,2,1,
		0,3,2
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float32) * NUM_VERTICES * 3, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
	m_vbos.push_back(vbo);

	GLuint uvVbo;
	glGenBuffers(1, &uvVbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float32) * NUM_UV, uvs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, nullptr);
	m_vbos.push_back(uvVbo);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * NUM_INDICES, indices, GL_STATIC_DRAW);
	m_vbos.push_back(ebo);
	m_numElements = NUM_INDICES;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


WaterRenderer::~WaterRenderer() {
	glDeleteBuffers(m_vbos.size(), m_vbos.data());
	glDeleteVertexArrays(1, &m_vao);
	glDeleteRenderbuffers(1, &m_refractionDepthRenderBuffer);
	glDeleteTextures(1, &m_refractionColorTexture);
	glDeleteFramebuffers(1, &m_refractionFBO);
}

void WaterRenderer::resizeFBO(uint32 width, uint32 height) {
	glDeleteRenderbuffers(1, &m_refractionDepthRenderBuffer);
	glDeleteTextures(1, &m_refractionColorTexture);
	glDeleteFramebuffers(1, &m_refractionFBO);
	buildFBO(width, height);
}

void WaterRenderer::buildFBO(uint32 width, uint32 height) {
	
	// Color
	glGenTextures(1, &m_refractionColorTexture);
	glBindTexture(GL_TEXTURE_2D, m_refractionColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Depth
	glGenRenderbuffers(1, &m_refractionDepthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_refractionDepthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	glGenFramebuffers(1, &m_refractionFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_refractionFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_refractionColorTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_refractionDepthRenderBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WaterRenderer::render(float32 x, float32 z, float32 scale) {
	m_waterShader->use();
	glBindVertexArray(m_vao);
	m_waterNormal->bind(Texture::UNIT_0);
	m_waterNormal2->bind(Texture::UNIT_1);
	m_waterDuDv->bind(Texture::UNIT_2);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_refractionColorTexture);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, m_y, z)) * glm::scale(glm::mat4(1.0f), glm::vec3(scale, 0.0f, scale));

	m_waterShader->setUniform("modelMatrix", modelMatrix);
	m_waterShader->setUniform("vpMatrix", RenderingContext::get()->camera.getViewProjectionMatrix());
	glDrawElements(GL_TRIANGLES, m_numElements, GL_UNSIGNED_INT, nullptr);
}
