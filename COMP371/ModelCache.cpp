#include "ModelCache.h"

#include <GL/glew.h>

#include <vector>

#include "Model.h"

ModelCache::ModelCache() {}


ModelCache::~ModelCache() {
	for (const auto& pair : m_models) {
		glDeleteVertexArrays(1, &pair.second->m_vao);
		glDeleteBuffers(pair.second->m_vbos.size(), pair.second->m_vbos.data());
		delete pair.second;
	}
	m_models.clear();
}

Model* ModelCache::loadModel(const std::string& name, const std::vector<float32> vertices, const std::vector<uint32> indices) {
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	std::vector<GLuint> vbos;
	
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float32) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
	vbos.push_back(vbo);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * indices.size(), indices.data(), GL_STATIC_DRAW);
	vbos.push_back(ebo);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_models[name] = new Model(vao, vbos, indices.size());
	return m_models[name];
}
