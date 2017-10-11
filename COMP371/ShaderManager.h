#pragma once

#include <unordered_map>
#include <string>

#include <GL/glew.h>

#include "ShaderProgram.h"

class ShaderManager {
private:
	static ShaderManager* s_instance;

	std::unordered_map<std::string, ShaderProgram*> m_shaders;

	ShaderManager();
	virtual ~ShaderManager();
public:
	ShaderProgram* createShaderProgram(const std::string name, std::string vertPath, const std::string fragPath);
	ShaderProgram* getShaderProgram(const std::string name) { return m_shaders[name]; };

	static void init() { s_instance = new ShaderManager(); };
	static ShaderManager* get() { return s_instance; };
	static void destroy() { delete s_instance; };
};

