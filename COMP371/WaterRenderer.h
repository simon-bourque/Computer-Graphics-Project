#pragma once

#include <vector>

#include "Types.h"

#include "Chunk.h"

#include "LightSource.h"

class ShaderProgram;
class Texture;

class WaterRenderer {
private:
	static WaterRenderer* s_instance;

	float32 m_textureTileFactor;

	ShaderProgram* m_waterShader;
	
	float32 m_y;

	uint32 m_vao;
	std::vector<uint32> m_vbos;
	uint32 m_numElements;


	Texture* m_waterNormal;
	Texture* m_waterNormal2;
	Texture* m_waterDuDv;

	uint32 m_refractionFBO;
	uint32 m_refractionDepthRenderBuffer;
	uint32 m_refractionColorTexture;
	
	WaterRenderer();
	virtual ~WaterRenderer();
public:

	void resizeFBO(uint32 width, uint32 height);
	void buildFBO(uint32 width, uint32 height);
	
	void prepare();
	void render(float32 x, float32 z, float32 scale);

	float32 getY() const { return m_y; };
	uint32 getRefractionFBO() const { return m_refractionFBO; };

	void setLightUniforms(const LightSource& light);

	static void init() { s_instance = new WaterRenderer(); };
	static WaterRenderer* get() { return s_instance; };
	static void destroy() { delete s_instance; };
};

