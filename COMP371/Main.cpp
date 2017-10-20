
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>

#include "Types.h"

#include "ShaderCache.h"

#include "Camera.h"

#include "RenderingContext.h"

#include "ModelCache.h"
#include "Model.h"

#include <vector>

GLFWwindow* initGLFW();
void update(float32 deltaSeconds);
void render();

// Test cube, will be removed later
#include "Primitives.h"
void initTestCube();
ShaderProgram* cubeShader = nullptr;
Model* cubeModel = nullptr;

int main() {

	// Initialize GLFW
	GLFWwindow* window = initGLFW();
	if (!window) {
		// Failed to load window
		return 1;
	}

	RenderingContext::init();

	initTestCube();

	RenderingContext::get()->camera.transform.translateLocal(0,0,2);

	// Start loop
	uint32 frames = 0;
	float64 counter = 0;
	float64 delta = 0;
	float64 currentTime = 0;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		currentTime = glfwGetTime();

		update((float32)delta);
		render();
		
		glfwSwapBuffers(window);

		delta = glfwGetTime() - currentTime;

		counter += delta;
		if (counter >= 1) {
			counter = 0;
			std::cout << "FPS: " << frames << std::endl;
			frames = 0;
		}
		else {
			frames++;
		}
	}

	glfwTerminate();

	RenderingContext::destroy();

	return 0;
}

GLFWwindow* initGLFW() {
	if (!glfwInit()) {
		std::cout << "Error: failed to initialize GLFW." << std::endl;
		return nullptr;
	}

	glfwDefaultWindowHints();

	GLFWwindow* window = glfwCreateWindow(640, 480, "Final Project", nullptr, nullptr);

	if (!window) {
		std::cout << "Error: failed to initialize window." << std::endl;
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int32 width, int32 height) -> void { glViewport(0, 0, width, height); });

	return window;
}

void update(float32 deltaSeconds) {
	// Update logic...
}

void render() {
	RenderingContext::get()->prepareFrame();

	// Render...


	// Render test cube
	cubeShader->use();

	cubeModel->bind();

	cubeShader->setUniform("mvpMatrix", RenderingContext::get()->camera.getViewProjectionMatrix());
	cubeShader->setUniform("color", glm::vec3(1, 0, 0));

	glDrawElements(GL_TRIANGLES, cubeModel->getCount(), GL_UNSIGNED_INT, nullptr);
}

void initTestCube() {
	std::vector<float32> vertices;
	std::vector<uint32> indices;

	cube::fill(vertices, indices);

	cubeModel = RenderingContext::get()->modelCache.loadModel("cube", vertices, indices);
	cubeShader = RenderingContext::get()->shaderCache.loadShaderProgram("test_cube", "test_cube_vert.glsl", "test_cube_frag.glsl");
}
