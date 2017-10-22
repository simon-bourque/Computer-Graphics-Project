#include "RenderingContext.h"

RenderingContext* RenderingContext::s_instance = nullptr;

RenderingContext::RenderingContext() {
	// Initialize glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		throw std::runtime_error("Error: failed to initialize glew.");
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


RenderingContext::~RenderingContext() {}

void RenderingContext::prepareFrame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera.updateViewProjectMatrix();
}

void RenderingContext::render() {

}
