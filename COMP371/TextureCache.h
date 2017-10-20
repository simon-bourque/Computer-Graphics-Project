#pragma once

#include <unordered_map>
#include <string>

class Texture;

class TextureCache {
	friend class RenderingContext;
private:
	std::unordered_map<std::string, Texture*> m_textures;

	TextureCache();
	virtual ~TextureCache();
public:

	Texture* loadTexture2D(const std::string& name, const std::string& imgPath);
	Texture* getTexture(const std::string& name) { return m_textures[name]; };
};

