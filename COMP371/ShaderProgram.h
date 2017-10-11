#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <string>

class ShaderProgram {
	friend class ShaderManager;
private:
	GLuint m_shaderHandle;

	ShaderProgram(GLuint shaderHandle);
public:
	virtual ~ShaderProgram();

	void use() const;

	void setUniform(const std::string& uniformName, const glm::mat4& matrix);
	void setUniform(const std::string& uniformName, const glm::vec3& vector);
};

