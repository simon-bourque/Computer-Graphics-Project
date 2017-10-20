#pragma once

#include "ShaderCache.h"
#include "ModelCache.h"

#include "Camera.h"

class RenderingContext {
private: // Member variables
	static RenderingContext* s_instance;
public:
	Camera camera;

	ShaderCache shaderCache;
	ModelCache modelCache;
private: // Member functions
	RenderingContext();
	virtual ~RenderingContext();
public:
	void prepareFrame();
	void render();

	static void init() { s_instance = new RenderingContext(); };
	static RenderingContext* get() { return s_instance; };
	static void destroy() { delete s_instance; };
};

