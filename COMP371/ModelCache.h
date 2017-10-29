#pragma once

#include <unordered_map>
#include <string>

#include "Types.h"

class Model;

class ModelCache {
	friend class RenderingContext;
private:
	std::unordered_map<std::string, Model*> m_models;
	
	ModelCache();
	virtual ~ModelCache();
public:

	Model* loadModel(const std::string& name, const std::vector<float32> vertices, const std::vector<uint32> indices);
	Model* getModel(const std::string& name) { return m_models[name]; };
};

