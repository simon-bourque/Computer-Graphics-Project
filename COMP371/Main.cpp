
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>

#include "Types.h"

#include "ShaderManager.h"

#include "Camera.h"

GLFWwindow* initGLFW();
void update(float32 deltaSeconds);
void render();

Camera camera(45.0f, 16.0f/9.0f);

// Test cube, will be removed later
#include "Primitives.h"
void initTestCube();
GLuint cubeVao = 0;
GLuint cubeVbo = 0;
GLuint cubeEbo = 0;
ShaderProgram* cubeShader = nullptr;

int main() {

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

	initTestCube();

	ShaderManager::init();
	cubeShader = ShaderManager::get()->createShaderProgram("test_cube", "test_cube_vert.glsl", "test_cube_frag.glsl");

	camera.transform.translateLocal(0,0,2);

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


	ShaderManager::destroy();

	glDeleteBuffers(1, &cubeVbo);
	glDeleteBuffers(1, &cubeEbo);
	glDeleteVertexArrays(1, &cubeVao);

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
	camera.updateViewProjectMatrix();

	// Render...


	// Render test cube
	cubeShader->use();

	glBindVertexArray(cubeVao);

	cubeShader->setUniform("mvpMatrix", camera.getViewProjectionMatrix());
	cubeShader->setUniform("color", glm::vec3(1, 0, 0));

	glDrawElements(GL_TRIANGLES, cube::numIndices, GL_UNSIGNED_INT, nullptr);
}

void initTestCube() {
	glGenVertexArrays(1, &cubeVao);
	glBindVertexArray(cubeVao);

	glGenBuffers(1, &cubeVbo);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float32) * cube::numVertices * 3, cube::vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);


	glGenBuffers(1, &cubeEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * cube::numIndices, cube::indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
