#include "ShaderManager.h"

#include <fstream>
#include <sstream>

std::string loadShaderText(const std::string& path);

ShaderManager* ShaderManager::s_instance = nullptr;

ShaderManager::ShaderManager() {}

ShaderManager::~ShaderManager() {
	for (const auto& pair : m_shaders) {
		glDeleteProgram(pair.second->m_shaderHandle);
		delete pair.second;
	}
	m_shaders.clear();
}

ShaderProgram* ShaderManager::createShaderProgram(const std::string name, std::string vertPath, const std::string fragPath) {
	GLint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Compile vertex shader
	std::string vertSrc = loadShaderText(vertPath);
	const GLchar* srcCString = vertSrc.c_str();

	glShaderSource(vertShader, 1, &srcCString, nullptr);
	glCompileShader(vertShader);

	GLint status = 0;
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		std::string msg("Failed to compile vertex shader: ");

		GLchar log[1024];
		GLsizei charsWritten = 0;
		glGetShaderInfoLog(vertShader, 1024, &charsWritten, log);

		msg.append(log);
		throw std::runtime_error(msg);
	}

	// Compile fragment shader
	std::string fragSrc = loadShaderText(fragPath);
	srcCString = fragSrc.c_str();

	glShaderSource(fragShader, 1, &srcCString, nullptr);
	glCompileShader(fragShader);

	status = 0;
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		std::string msg("Failed to compile fragment shader: ");

		GLchar log[1024];
		GLsizei charsWritten = 0;
		glGetShaderInfoLog(fragShader, 1024, &charsWritten, log);

		msg.append(log);
		throw std::runtime_error(msg);
	}

	// Link shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);

	status = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		std::string msg("Failed to link shader program: ");

		GLchar log[1024];
		GLsizei charsWritten = 0;
		glGetProgramInfoLog(shaderProgram, 1024, &charsWritten, log);

		msg.append(log);
		throw std::runtime_error(msg);
	}

	// Clean up.
	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	m_shaders[name] = new ShaderProgram(shaderProgram);
	
	return m_shaders[name];
}

std::string loadShaderText(const std::string& path) {
	std::ifstream input(path);

	if (!input) {
		input.close();
		throw std::runtime_error("Error: could not find file \'" + path + "\'.");
	}

	std::stringstream ss;
	ss << input.rdbuf();

	input.close();

	return ss.str();
}
