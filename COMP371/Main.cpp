//Standard library
#include <iostream>

//External includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Profiling.h"

//Local headers
#include "ChunkManager.h"
#include "Types.h"

#include "ShaderCache.h"

#include "Camera.h"

#include "RenderingContext.h"

#include "LightSource.h"
#include "ShadowMap.h"

#include "ModelCache.h"
#include "Model.h"

#include "TextureCache.h"
#include "Texture.h"

#include <vector>

#include "FreeCameraController.h"

#include "WaterRenderer.h"

GLFWwindow* initGLFW();
void update(float32 deltaSeconds);
void render();

// Test cube, will be removed later
#include "Primitives.h"
void initTestCube();
ShaderProgram* cubeShader = nullptr;
Model* cubeModel = nullptr;
Texture* cubeTexture = nullptr;

int32 SCREENWIDTH = 600, SCREENHEIGHT = 480;
glm::vec3 playerPosition(0, 160, 0);
LightSource* sun = nullptr;
ShadowMap* shadowMap = nullptr;
glm::vec3 lightDirection(0.0f, -0.5f, -0.5f);

ShaderProgram* chunkShader = nullptr;
Texture* chunkTexture = nullptr;
ShaderProgram* smShader = nullptr;

void initSkybox();
unsigned int skyboxVAO, skyboxVBO;
ShaderProgram* skyboxShader = nullptr;
Model* skyboxModel = nullptr;
Texture* skyboxTexture = nullptr;

GLFWwindow* gWindow = nullptr;
FreeCameraController* gCameraController;

//#define COMPILE_DRAW_NORMALS // Uncomment me if you want to render normals

#ifdef COMPILE_DRAW_NORMALS
ShaderProgram* chunkNormalsShader = nullptr;
#endif

#define RENDER_WATER // Comment me if you don't want to render water

int main() {

	try {
		
		// Initialize GLFW
		gWindow = initGLFW();

		RenderingContext::init();
		chunkShader = RenderingContext::get()->shaderCache.loadShaderProgram("chunk_shader", "chunk_vert.glsl", "chunk_frag.glsl");
		smShader = RenderingContext::get()->shaderCache.loadShaderProgram("sm_shader", "shadowmap_vert.glsl", "shadowmap_frag.glsl");
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

#ifdef COMPILE_DRAW_NORMALS
		chunkNormalsShader = RenderingContext::get()->shaderCache.loadShaderProgram("chunk_shader", "chunk_vert.glsl", "chunk_normals_frag.glsl", "chunk_normals_geo.glsl");
		//glLineWidth(3.0f);
#endif
		initTestCube();

		WaterRenderer::init();
		WaterRenderer::get()->buildFBO(SCREENWIDTH, SCREENHEIGHT);
	}
	catch (std::runtime_error& ex) {
		std::cout << ex.what() << std::endl;
		system("pause");
		return 1;
	}

	gCameraController = new FreeCameraController(&RenderingContext::get()->camera);

	RenderingContext::get()->camera.transform.translateLocal(playerPosition.x, playerPosition.y, playerPosition.z);
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

	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

	sun = new LightSource(lightDirection, lightColor);
	shadowMap = new ShadowMap(SCREENWIDTH, SCREENHEIGHT, lightDirection);

	WaterRenderer::get()->setLightUniforms(*sun);

	// Start loop
	uint32 frames = 0;
	float64 counter = 0;
	float64 delta = 0;
	float64 currentTime = 0;

	while (!glfwWindowShouldClose(gWindow)) {

#ifdef DEBUG_BUILD
		BROFILER_FRAME("MainThread")
#endif
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
	delete shadowMap;
	delete sun;

	WaterRenderer::destroy();
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

	GLFWwindow* window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Final Project", nullptr, nullptr);

	if (!window) {
		glfwTerminate();
		throw std::runtime_error("Error: failed to initialize window.");
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int32 width, int32 height) -> void { 
		glViewport(0, 0, width, height);
		WaterRenderer::get()->resizeFBO(width, height);
		shadowMap->updateSize(width, height);
		SCREENWIDTH = width;
		SCREENHEIGHT = height;
	});

	return window;
}

void update(float32 deltaSeconds) {
	INSTRUMENT_FUNCTION("Update", Profiler::Color::Orchid);

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

	chunkShader->use();
	chunkShader->setUniform("viewPos", playerPos);

	static float32 panner = 0;
	panner += 0.05 * deltaSeconds;
	RenderingContext::get()->shaderCache.getShaderProgram("water_shader")->use();
	RenderingContext::get()->shaderCache.getShaderProgram("water_shader")->setUniform("viewPos", playerPos);
	RenderingContext::get()->shaderCache.getShaderProgram("water_shader")->setUniform("panner", panner);
}

void render() {
	INSTRUMENT_FUNCTION("Render", Profiler::Color::Bisque);

	RenderingContext::get()->prepareFrame();

#ifdef COMPILE_DRAW_NORMALS
	chunkNormalsShader->use();
	chunkNormalsShader->setUniform("vpMatrix", RenderingContext::get()->camera.getViewProjectionMatrix());
#endif

	const std::unordered_map<int64, Chunk>& chunks = ChunkManager::instance()->getCurrentlyLoadedChunks();
	shadowMap->updateMvp(lightDirection);

	//First Pass (Shadows)
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMap->getFbo());
	smShader->use();
	smShader->setUniform("lightSpaceMatrix", shadowMap->getMvp());
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	for (const auto& chunk : chunks) {
		glBindVertexArray(chunk.second.getVao());
		glDrawElementsInstanced(GL_TRIANGLES, cube::numIndices, GL_UNSIGNED_INT, nullptr, chunk.second.getBlockCount());
	}
	glDisable(GL_POLYGON_OFFSET_FILL);

	// Second Pass (render refraction texture)
	chunkShader->use();
	chunkShader->setUniform("vpMatrix", RenderingContext::get()->camera.getViewProjectionMatrix());
	chunkShader->setUniform("waterPlaneHeight", WaterRenderer::get()->getY());
	chunkShader->setUniform("lightSpaceMatrix", shadowMap->getMvp());
	chunkTexture->bind(Texture::UNIT_0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadowMap->getTexture());
	glBindFramebuffer(GL_FRAMEBUFFER, WaterRenderer::get()->getRefractionFBO());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_DISTANCE0);
	for (const auto& chunk : chunks) {
		glBindVertexArray(chunk.second.getVao());
		glDrawElementsInstanced(GL_TRIANGLES, cube::numIndices, GL_UNSIGNED_INT, nullptr, chunk.second.getBlockCount());
	}
	glDisable(GL_CLIP_DISTANCE0);

	
	// Render chunks
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadowMap->getTexture());
	for (const auto& chunk : chunks) {
		glBindVertexArray(chunk.second.getVao());
		glDrawElementsInstanced(GL_TRIANGLES, cube::numIndices, GL_UNSIGNED_INT, nullptr, chunk.second.getBlockCount());

#ifdef COMPILE_DRAW_NORMALS
		chunkNormalsShader->use();
		glDrawElementsInstanced(GL_POINTS, cube::numIndices, GL_UNSIGNED_INT, nullptr, chunk.second.getBlockCount());
		chunkShader->use();
#endif
	}

	// Render water
#ifdef RENDER_WATER
	for (const auto& chunk : chunks) {
		glm::vec3 pos = chunk.second.getPosition();

		// Render water
		WaterRenderer::get()->render(pos.x, pos.z, ChunkManager::CHUNKWIDTH);
	}
#endif

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
