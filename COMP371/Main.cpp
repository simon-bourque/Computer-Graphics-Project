//Standard library
#include <iostream>

//External includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

//Local headers
#include "ChunkManager.h"
#include "Types.h"

#include "ShaderCache.h"

#include "Camera.h"

#include "RenderingContext.h"

#include "LightSource.h"

#include "ModelCache.h"
#include "Model.h"

#include "TextureCache.h"
#include "Texture.h"

#include <vector>

#include "FreeCameraController.h"
#include "Player.h"
#include "AABB.h"

GLFWwindow* initGLFW();
void update(float32 deltaSeconds);
void render();

// Test cube, will be removed later
#include "Primitives.h"
void initTestCube();
ShaderProgram* cubeShader = nullptr;
Model* cubeModel = nullptr;
Texture* cubeTexture = nullptr;

glm::vec3 playerPosition(0, 160, 2);

ShaderProgram* chunkShader = nullptr;
Texture* chunkTexture = nullptr;

GLFWwindow* gWindow = nullptr;
FreeCameraController* gCameraController;
Player* gPlayer;

//#define COMPILE_DRAW_NORMALS // Uncomment me if you want to render normals

#ifdef COMPILE_DRAW_NORMALS
ShaderProgram* chunkNormalsShader = nullptr;
#endif

int main() {

	try {
		
		// Initialize GLFW
		gWindow = initGLFW();

		RenderingContext::init();
		chunkShader = RenderingContext::get()->shaderCache.loadShaderProgram("chunk_shader", "chunk_vert.glsl", "chunk_frag.glsl");

#ifdef COMPILE_DRAW_NORMALS
		chunkNormalsShader = RenderingContext::get()->shaderCache.loadShaderProgram("chunk_shader", "chunk_vert.glsl", "chunk_normals_frag.glsl", "chunk_normals_geo.glsl");
		//glLineWidth(3.0f);
#endif

		chunkTexture = RenderingContext::get()->textureCache.loadTexture2DArray("texture_shader", 7, "tiles.png");
	}
	catch (std::runtime_error& ex) {
		std::cout << ex.what() << std::endl;
		system("pause");
		return 1;
	}

	gCameraController = new FreeCameraController(&RenderingContext::get()->camera);

	// Player
	gPlayer = new Player();
	gPlayer->transform.translateLocal(0, 160, 2);

	//RenderingContext::get()->camera.transform.translateLocal(0,160,2);
	RenderingContext::get()->camera.transform.orient(glm::degrees(-0.0f), 0, 0);

	// Load face data for shader
	std::vector<glm::vec3> faceData;
	faceData.push_back(glm::vec3(1, 0, 2)); // Grass
	faceData.push_back(glm::vec3(3, 3, 3)); // Log
	faceData.push_back(glm::vec3(4, 4, 4)); // Leaves
	faceData.push_back(glm::vec3(2, 2, 2)); // Dirt
	faceData.push_back(glm::vec3(5, 5, 5)); // Sand
	faceData.push_back(glm::vec3(6, 6, 6)); // Snow
	chunkShader->use();
	chunkShader->setUniform("faceData", faceData);

	glm::vec3 lightDirection(-0.80f, -0.5f, 0.0f);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	LightSource sun(lightDirection, lightColor, 0.5f, 0.25f);
	chunkShader->use();
	chunkShader->setUniform("lightColor", sun.getColor());
	chunkShader->setUniform("lightDirection", sun.getDirection());
	chunkShader->setUniform("ambientStrength", sun.getAmbStrength());
	chunkShader->setUniform("specularStrength", sun.getSpecStrength());

	initTestCube();

	// Start loop
	uint32 frames = 0;
	float64 counter = 0;
	float64 delta = 0;
	float64 currentTime = 0;

	//AABB b1(glm::vec3(0.0f,0.0f,0.0f), 1);
	//AABB b2(glm::vec3(0.5f,1.0f,1.0f), 1);

	//bool bb = AABB::checkCollision(b1, b2);
	//std::cout << "col1 " << b << std::endl;

	//bool bp = AABB::checkPointInAABB(glm::vec3(0.1f,0.1f,0.1f), b1);
	//std::cout << "col2 " << bp << std::endl;

	while (!glfwWindowShouldClose(gWindow)) {
		glfwPollEvents();

		currentTime = glfwGetTime();

		update((float32)delta);
		render();
		
		glfwSwapBuffers(gWindow);

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

	delete gCameraController;
	delete gPlayer;

	RenderingContext::destroy();
	
	glfwTerminate();


	return 0;
}

GLFWwindow* initGLFW() {
	if (!glfwInit()) {
		throw std::runtime_error("Error: failed to initialize GLFW.");
	}

	glfwDefaultWindowHints();
	// 8x MSAA
	glfwWindowHint(GLFW_SAMPLES, 8);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Final Project", nullptr, nullptr);

	if (!window) {
		glfwTerminate();
		throw std::runtime_error("Error: failed to initialize window.");
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int32 width, int32 height) -> void { glViewport(0, 0, width, height); });

	return window;
}

void update(float32 deltaSeconds) {
	// Update logic...
	gCameraController->update(deltaSeconds);
	gPlayer->update(deltaSeconds);


	RenderingContext::get()->camera.transform = gPlayer->transform;
	RenderingContext::get()->camera.transform.translateLocal(0.0f, 2.0f, 0.0f);
	//RenderingContext::get()->camera.transform.xPos = gPlayer->transform.xPos;
	//RenderingContext::get()->camera.transform.yPos = gPlayer->transform.yPos + 2.0f;
	//RenderingContext::get()->camera.transform.zPos = gPlayer->transform.zPos;

	glm::vec3 currentChunk = ChunkManager::instance()->getCurrentChunk(gPlayer->getPosition());

	// Spooky hack lol
	static glm::vec3 lastChunk(currentChunk.x + 1.0f, currentChunk.y, currentChunk.z);

	if (currentChunk != lastChunk) {
		ChunkManager::instance()->loadChunks(currentChunk);
		lastChunk = currentChunk;
	}
	ChunkManager::instance()->uploadQueuedChunk();

	chunkShader->use();
	chunkShader->setUniform("viewPos", gPlayer->getPosition());
}

void render() {
	RenderingContext::get()->prepareFrame();

#ifdef COMPILE_DRAW_NORMALS
	chunkNormalsShader->use();
	chunkNormalsShader->setUniform("vpMatrix", RenderingContext::get()->camera.getViewProjectionMatrix());
#endif

	// Render chunks
	chunkShader->use();
	chunkShader->setUniform("vpMatrix", RenderingContext::get()->camera.getViewProjectionMatrix());
	chunkTexture->bind(Texture::UNIT_0);
	const std::unordered_map<int64, Chunk>& chunks = ChunkManager::instance()->getCurrentlyLoadedChunks();
	for (const auto& chunk : chunks) {
		glBindVertexArray(chunk.second.getVao());
		glDrawElementsInstanced(GL_TRIANGLES, cube::numIndices, GL_UNSIGNED_INT, nullptr, chunk.second.getBlockCount());

#ifdef COMPILE_DRAW_NORMALS
		chunkNormalsShader->use();
		glDrawElementsInstanced(GL_POINTS, cube::numIndices, GL_UNSIGNED_INT, nullptr, chunk.second.getBlockCount());
		chunkShader->use();
#endif
	}

	// Render test cube
	cubeShader->use();
	
	cubeModel->bind();
	
	cubeShader->setUniform("mvpMatrix", RenderingContext::get()->camera.getViewProjectionMatrix());
	cubeShader->setUniform("color", glm::vec3(1, 0, 0));
	
	glDrawElements(GL_TRIANGLES, cubeModel->getCount(), GL_UNSIGNED_INT, nullptr);
}

void initTestCube() {
	std::vector<float32> vertices;
	std::vector<float32> uvCoords;
	std::vector<float32> normals;
	std::vector<uint32> indices;

	cube::fill(vertices, uvCoords, normals, indices);

	cubeModel = RenderingContext::get()->modelCache.loadModel("cube", vertices, indices);
	cubeShader = RenderingContext::get()->shaderCache.loadShaderProgram("test_cube", "test_cube_vert.glsl", "test_cube_frag.glsl");
	cubeTexture = RenderingContext::get()->textureCache.loadTexture2D("test_cube_texture", "test.png");
}

glm::vec2 getMouseAxis() {
	// Poll mouse position
	float64 mPosX;
	float64 mPosY;
	glfwGetCursorPos(gWindow, &mPosX, &mPosY);

	static float64 lastMousePosX = mPosX;
	static float64 lastMousePosY = mPosY;

	glm::vec2 mouseAxis(0, 0);

	if (mPosX != lastMousePosX || mPosY != lastMousePosY) {
		mouseAxis.x = mPosX - lastMousePosX;
		mouseAxis.y = lastMousePosY - mPosY;
	}

	lastMousePosX = mPosX;
	lastMousePosY = mPosY;

	return mouseAxis;
}
