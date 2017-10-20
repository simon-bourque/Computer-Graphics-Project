#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram(GLuint shaderHandle) : m_shaderHandle(shaderHandle) {}
ShaderProgram::~ShaderProgram() {}

void ShaderProgram::use() const {
	glUseProgram(m_shaderHandle);
}

void ShaderProgram::setUniform(const std::string& uniformName, const glm::mat4& matrix) {
	glUniformMatrix4fv(glGetUniformLocation(m_shaderHandle, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::setUniform(const std::string& uniformName, const glm::vec3& vector) {
	glUniform3fv(glGetUniformLocation(m_shaderHandle, uniformName.c_str()), 1, glm::value_ptr(vector));
}
