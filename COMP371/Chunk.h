#pragma once
//Standard Library
#include <vector>

//External includes
#include "glm\glm.hpp"

//Local headers
#include "Types.h"

struct Chunk {
	//Constructor
	Chunk(glm::vec3 position, uint32 VAO, std::vector<uint32> VBOs)
		: m_position(position)
		, m_VAO(VAO)
		, m_VBOs(VBOs)
	{}

	//Getters
	glm::vec3 getPosition() { return m_position; }
	uint32 getVao() { return m_VAO; }
	std::vector<uint32> getVbos() { return m_VBOs; }

private:
	//OpenGL Buffers
	uint32 m_VAO;					//VAO handle for all the VBOs (blocks) in the chunk
	std::vector<uint32> m_VBOs; 	//Vector of all block data

	//World coordinate of the chunk (center).
	glm::vec3 m_position;
};