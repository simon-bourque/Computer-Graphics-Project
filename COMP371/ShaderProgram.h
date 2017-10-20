#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <string>

class ShaderProgram {
	friend class ShaderCache;
private:
	GLuint m_shaderHandle;

	ShaderProgram(GLuint shaderHandle);
	virtual ~ShaderProgram();
public:
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

	void use() const;

	void setUniform(const std::string& uniformName, const glm::mat4& matrix);
	void setUniform(const std::string& uniformName, const glm::vec3& vector);
};

