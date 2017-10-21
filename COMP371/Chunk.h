#pragma once
//Standard Library
#include <vector>

//External includes
#include "glm\glm.hpp"

//Local headers
#include "Types.h"

struct Chunk
{
	//Constructor
	Chunk(glm::vec3 position)
		: m_position(position)
	{}

	//Getters
	glm::vec3 getPosition() const { return m_position; }
	uint32 getVao() const { return m_VAO; }
	std::vector<uint32> getVbos() const { return m_VBOs; }

	//Setters
	void setVao(uint32 VAO) { m_VAO = VAO; }
	void setVbos(std::vector<uint32> VBOs) { m_VBOs = VBOs; }

private:
	//OpenGL Buffers
	uint32 m_VAO;					//VAO handle for all the VBOs (blocks) in the chunk
	std::vector<uint32> m_VBOs; 	//Vector of all block data

	//World coordinate of the chunk (center).
	glm::vec3 m_position;
};