#pragma once

#include "Types.h"
#include "ShaderProgram.h"

#include "glm\glm.hpp"
class ShadowMap
{
public:
	ShadowMap(uint32 width, uint32 height, glm::vec3 lightDirection);

	void updateSize(int32 width, int32 height);
	void updateMvp(glm::vec3 lightDirection);

	uint32 getFbo() { return m_fbo; }
	glm::mat4 getMvp() { return m_MVP; }

	~ShadowMap();
private:
	void buildBuffer(int32 width, int32 height);

	//Texture dimensions
	uint32 m_width;
	uint32 m_height;

	glm::mat4 m_MVP;

	ShaderProgram* m_Shaders;

	//OpenGL IDs
	uint32 m_fbo;
	uint32 m_texture;
};

