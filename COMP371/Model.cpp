#include "Model.h"

Model::Model(GLuint vao, const std::vector<GLuint>& vbos, uint32 count) : m_vao(vao), m_vbos(vbos), m_count(count) {}

Model::~Model() {}

void Model::bind() {
	glBindVertexArray(m_vao);
}
