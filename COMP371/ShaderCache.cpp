#include "ShaderCache.h"

#include <fstream>
#include <sstream>

std::string loadShaderText(const std::string& path);
std::string getShaderTypeString(GLenum type);

ShaderCache::ShaderCache() {}

ShaderCache::~ShaderCache() {
	for (const auto& pair : m_shaders) {
		glDeleteProgram(pair.second->m_shaderHandle);
		delete pair.second;
	}
	m_shaders.clear();
}

ShaderProgram* ShaderCache::loadShaderProgram(const std::string& name, const std::string& vertPath, const std::string& fragPath) {
	GLint vertShader = compileShader(loadShaderText(vertPath), GL_VERTEX_SHADER);
	GLint fragShader = compileShader(loadShaderText(fragPath), GL_FRAGMENT_SHADER);

	// Link shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);

	GLint status = 0;
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

ShaderProgram* ShaderCache::loadShaderProgram(const std::string& name, const std::string& vertPath, const std::string& fragPath, const std::string& geoPath) {
	GLint vertShader = compileShader(loadShaderText(vertPath), GL_VERTEX_SHADER);
	GLint geoShader = compileShader(loadShaderText(geoPath), GL_GEOMETRY_SHADER);
	GLint fragShader = compileShader(loadShaderText(fragPath), GL_FRAGMENT_SHADER);

	// Link shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, geoShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);

	GLint status = 0;
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
	glDetachShader(shaderProgram, geoShader);
	glDetachShader(shaderProgram, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(geoShader);
	glDeleteShader(fragShader);

	m_shaders[name] = new ShaderProgram(shaderProgram);

	return m_shaders[name];
}

GLint ShaderCache::compileShader(const std::string& source, GLenum type) const {
	// Compile shader
	const GLchar* srcCString = source.c_str();
	GLint shader = glCreateShader(type);

	glShaderSource(shader, 1, &srcCString, nullptr);
	glCompileShader(shader);

	GLint status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		std::string msg("Failed to compile " + getShaderTypeString(type) + " shader: ");

		GLchar log[1024];
		GLsizei charsWritten = 0;
		glGetShaderInfoLog(shader, 1024, &charsWritten, log);

		msg.append(log);
		throw std::runtime_error(msg);
	}

	return shader;
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

std::string getShaderTypeString(GLenum type) {
	switch (type) {
	case GL_VERTEX_SHADER:
		return "vertex";
	case GL_FRAGMENT_SHADER:
		return "fragment";
	case GL_GEOMETRY_SHADER:
		return "geometry";
	default:
		return "unknown";
	}
}
