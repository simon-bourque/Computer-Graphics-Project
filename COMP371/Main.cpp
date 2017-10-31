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

#include "ModelCache.h"
#include "Model.h"

#include "TextureCache.h"
#include "Texture.h"

#include <vector>

#include "FreeCameraController.h"

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

void initSkybox();
unsigned int skyboxVAO, skyboxVBO;
ShaderProgram* skyboxShader = nullptr;
Model* skyboxModel = nullptr;
Texture* skyboxTexture = nullptr;

GLFWwindow* gWindow = nullptr;
FreeCameraController* gCameraController;

int main() {

	try {
		
		// Initialize GLFW
		gWindow = initGLFW();

		RenderingContext::init();
		chunkShader = RenderingContext::get()->shaderCache.loadShaderProgram("chunk_shader", "chunk_vert.glsl", "chunk_frag.glsl");
		chunkTexture = RenderingContext::get()->textureCache.loadTexture2DArray("chunk_texture", 7, "tiles.png");

		// Load skybox texture
		CubeMapPaths paths;
		paths.bk = "craterlake_bk.tga";
		paths.ft = "craterlake_ft.tga";
		paths.up = "craterlake_up.tga";
		paths.dn = "craterlake_dn.tga";
		paths.lf = "craterlake_lf.tga";
		paths.rt = "craterlake_rt.tga";
		skyboxShader = RenderingContext::get()->shaderCache.loadShaderProgram("skybox_shader", "skybox_vert.glsl", "skybox_frag.glsl");
		skyboxTexture = RenderingContext::get()->textureCache.loadTextureCubeMap("skybox_texture", paths);

		initSkybox();
		initTestCube();
	}
	catch (std::runtime_error& ex) {
		std::cout << ex.what() << std::endl;
		system("pause");
		return 1;
	}
	
	ChunkManager::instance()->loadChunks(playerPosition);

	gCameraController = new FreeCameraController(&RenderingContext::get()->camera);

	RenderingContext::get()->camera.transform.translateLocal(0,160,2);
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

	// Start loop
	uint32 frames = 0;
	float64 counter = 0;
	float64 delta = 0;
	float64 currentTime = 0;

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

	RenderingContext::destroy();
	
	glfwTerminate();


	return 0;
}

GLFWwindow* initGLFW() {
	if (!glfwInit()) {
		throw std::runtime_error("Error: failed to initialize GLFW.");
	}

	glfwDefaultWindowHints();

	GLFWwindow* window = glfwCreateWindow(640, 480, "Final Project", nullptr, nullptr);

	if (!window) {
		glfwTerminate();
		throw std::runtime_error("Error: failed to initialize window.");
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int32 width, int32 height) -> void { glViewport(0, 0, width, height); });

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) -> void { gCameraController->onKey(key, action); });
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) -> void { gCameraController->onMouseButton(button, action); });

	return window;
}

void update(float32 deltaSeconds) {
	// Update logic...
	gCameraController->update(deltaSeconds);

	const Transform& playerTransform = RenderingContext::get()->camera.transform;
	glm::vec3 playerPos(playerTransform.xPos, playerTransform.yPos, playerTransform.zPos);
	glm::vec3 currentChunk = ChunkManager::instance()->getCurrentChunk(playerPos);

	// Spooky hack lol
	static glm::vec3 lastChunk(currentChunk.x + 1.0f, currentChunk.y, currentChunk.z);

	if (currentChunk != lastChunk) {
		ChunkManager::instance()->loadChunks(currentChunk);
		lastChunk = currentChunk;
	}
	ChunkManager::instance()->uploadQueuedChunk();
}

void render() {
	RenderingContext::get()->prepareFrame();

	// Render chunks
	chunkShader->use();
	chunkShader->setUniform("vpMatrix", RenderingContext::get()->camera.getViewProjectionMatrix());
	chunkTexture->bind(Texture::UNIT_0);
	const std::unordered_map<int64, Chunk>& chunks = ChunkManager::instance()->getCurrentlyLoadedChunks();
	for (const auto& chunk : chunks) {
		glBindVertexArray(chunk.second.getVao());
		glDrawElementsInstanced(GL_TRIANGLES, cube::numIndices, GL_UNSIGNED_INT, nullptr, chunk.second.getBlockCount());
	}

	// Render test cube
	cubeShader->use();
	
	cubeModel->bind();
	
	cubeShader->setUniform("mvpMatrix", RenderingContext::get()->camera.getViewProjectionMatrix());
	cubeShader->setUniform("color", glm::vec3(1, 0, 0));
	
	glDrawElements(GL_TRIANGLES, cubeModel->getCount(), GL_UNSIGNED_INT, nullptr);

	// Render skybox
	glDepthFunc(GL_LEQUAL);
	skyboxShader->use();
	skyboxShader->setUniform("view", glm::mat4(glm::mat3(RenderingContext::get()->camera.getViewMatrix())));
	skyboxShader->setUniform("projection", RenderingContext::get()->camera.getProjectionMatrix());
	skyboxTexture->bind(Texture::UNIT_0);
	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS); // set depth function back to default
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

void initSkybox() {
	float skyboxVertices[] = {
		// positions          
		-512.0f,  512.0f, -512.0f,
		-512.0f, -512.0f, -512.0f,
		512.0f, -512.0f, -512.0f,
		512.0f, -512.0f, -512.0f,
		512.0f,  512.0f, -512.0f,
		-512.0f,  512.0f, -512.0f,

		-512.0f, -512.0f,  512.0f,
		-512.0f, -512.0f, -512.0f,
		-512.0f,  512.0f, -512.0f,
		-512.0f,  512.0f, -512.0f,
		-512.0f,  512.0f,  512.0f,
		-512.0f, -512.0f,  512.0f,

		512.0f, -512.0f, -512.0f,
		512.0f, -512.0f,  512.0f,
		512.0f,  512.0f,  512.0f,
		512.0f,  512.0f,  512.0f,
		512.0f,  512.0f, -512.0f,
		512.0f, -512.0f, -512.0f,

		-512.0f, -512.0f,  512.0f,
		-512.0f,  512.0f,  512.0f,
		512.0f,  512.0f,  512.0f,
		512.0f,  512.0f,  512.0f,
		512.0f, -512.0f,  512.0f,
		-512.0f, -512.0f,  512.0f,

		-512.0f,  512.0f, -512.0f,
		512.0f,  512.0f, -512.0f,
		512.0f,  512.0f,  512.0f,
		512.0f,  512.0f,  512.0f,
		-512.0f,  512.0f,  512.0f,
		-512.0f,  512.0f, -512.0f,

		-512.0f, -512.0f, -512.0f,
		-512.0f, -512.0f,  512.0f,
		512.0f, -512.0f, -512.0f,
		512.0f, -512.0f, -512.0f,
		-512.0f, -512.0f,  512.0f,
		512.0f, -512.0f,  512.0f
	};

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);\
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
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
