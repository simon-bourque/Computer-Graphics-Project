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
	const std::vector<glm::vec3>& getBlockPositions() const { return m_voxel_positions; };
	const std::vector<glm::vec3>& getFoliageBlockPositions() const { return m_foliage_voxel_positions; };

	//Setters
	void setVao(uint32 VAO) { m_VAO = VAO; }
	void setVbos(const std::vector<uint32>& VBOs) { m_VBOs = VBOs; }
	void setBlockCount(uint32 blockCount) { m_blockCount = blockCount; };
	void setBlockPositions(const std::vector<glm::vec3>& positions) { m_voxel_positions = positions; };
	void setFoliageBlockPositions(const std::vector<glm::vec3>& positions) { m_foliage_voxel_positions = positions; };

private:
	//OpenGL Buffers
	uint32 m_VAO;								//VAO handle for all the VBOs (blocks) in the chunk
	std::vector<uint32> m_VBOs;					//Vector of all block data

	glm::vec3 m_position;						//World coordinate of the chunk (center).
	std::vector<glm::vec3> m_voxel_positions;	//Stores positions of the voxels.
	std::vector<glm::vec3> m_foliage_voxel_positions;

	uint32 m_blockCount;
};