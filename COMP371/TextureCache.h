#pragma once

#include <unordered_map>
#include <string>

#include "Types.h"

class Texture;

struct CubeMapPaths {
	std::string bk; // Back
	std::string ft; // Front
	std::string up; // Up
	std::string dn; // Down
	std::string lf; // Left
	std::string rt; // Right
};

class TextureCache {
	friend class RenderingContext;
private:
	std::unordered_map<std::string, Texture*> m_textures;

	TextureCache();
	virtual ~TextureCache();
public:

	Texture* loadTexture2D(const std::string& name, const std::string& imgPath);
	Texture* loadTexture2DArray(const std::string& name, uint32 numTiles, const std::string& imgPath);
	Texture* loadTextureCubeMap(const std::string& name, const CubeMapPaths& imgPaths);
	Texture* getTexture(const std::string& name) { return m_textures[name]; };
};

