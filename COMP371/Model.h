#pragma once

#include <GL/glew.h>

#include <vector>

#include "Types.h"

class Model {
	friend class ModelCache;
private:
	GLuint m_vao;
	std::vector<GLuint> m_vbos;
	uint32 m_count;

	Model(GLuint vao, const std::vector<GLuint>& vbos, uint32 count);
	virtual ~Model();
	
public:
	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	void bind();

	uint32 getCount() const { return m_count; };
};

