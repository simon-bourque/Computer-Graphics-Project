//Standard library
#include <iostream>

//External includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

//Local headers
#include "ChunkManager.h"
#include "Types.h"

GLFWwindow* initGLFW();
void update(float32 deltaSeconds);
void render();

glm::vec3 playerPosition(0.0f);

int main() {

	ChunkManager::instance()->loadChunks(playerPosition);

	// Initialize GLFW
	GLFWwindow* window = initGLFW();
	if (!window) {
		// Failed to load window
		return 1;
	}

	// Initialize glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Error: failed to initialize glew." << std::endl;
		return 1;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render...
}
