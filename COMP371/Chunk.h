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
	{};

	Chunk() {};

	//Getters
	glm::vec3 getPosition() const { return m_position; }
	uint32 getVao() const { return m_VAO; }
	const std::vector<uint32>& getVbos() const { return m_VBOs; }
	uint32 getBlockCount() const { return m_blockCount; };

	//Setters
	void setVao(uint32 VAO) { m_VAO = VAO; }
	void setVbos(const std::vector<uint32>& VBOs) { m_VBOs = VBOs; }
	void setBlockCount(uint32 blockCount) { m_blockCount = blockCount; };

private:
	//OpenGL Buffers
	uint32 m_VAO;					//VAO handle for all the VBOs (blocks) in the chunk
	std::vector<uint32> m_VBOs; 	//Vector of all block data

	//World coordinate of the chunk (center).
	glm::vec3 m_position;

	uint32 m_blockCount;
};