#pragma once

#include <unordered_map>
#include <string>

#include <GL/glew.h>

#include "ShaderProgram.h"

class ShaderCache {
	friend class RenderingContext;
private:
	std::unordered_map<std::string, ShaderProgram*> m_shaders;

	ShaderCache();
	virtual ~ShaderCache();

	GLint compileShader(const std::string& source, GLenum type) const;
public:
	
	ShaderProgram* loadShaderProgram(const std::string& name, const std::string& vertPath, const std::string& fragPath);
	ShaderProgram* loadShaderProgram(const std::string& name, const std::string& vertPath, const std::string& fragPath, const std::string& geoPath);
	ShaderProgram* getShaderProgram(const std::string& name) { return m_shaders[name]; };
};

