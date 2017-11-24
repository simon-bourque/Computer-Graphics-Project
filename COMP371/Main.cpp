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

#include "InputManager.h"

#include "FreeCameraController.h"
#include "Player.h"
#include "Collision.h"

#include "WaterRenderer.h"

#include "InputManager.h"

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
glm::vec3 lightDirection(0.0f, -0.2f, -0.5f);

ShaderProgram* chunkShader = nullptr;
Texture* chunkTexture = nullptr;

void initSkybox();
unsigned int skyboxVAO, skyboxVBO;
ShaderProgram* skyboxShader = nullptr;
Model* skyboxModel = nullptr;
Texture* skyboxTexture = nullptr;

GLFWwindow* gWindow = nullptr;

bool FREE_CAM_ON = false;
glm::vec3 camPositionVector;
FreeCameraController* gCameraController;
bool PLAYER_COLLISION_AABB = false;
Player* gPlayer;

bool gFullscreen = false;

//#define COMPILE_DRAW_NORMALS // Uncomment me if you want to render normals

#ifdef COMPILE_DRAW_NORMALS
ShaderProgram* chunkNormalsShader = nullptr;
#endif

#define RENDER_WATER	// Comment me if you don't want to render water
#define RENDER_SHADOWS	// Comment me if you don't want to render shadows

int main() {

	try {
		
		// Initialize GLFW
		gWindow = initGLFW();

		// Set fullscreen toggle
		InputManager::instance()->registerKeyCallback([](int32 key, int32 action) { 
			if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
				if (gFullscreen) {
					const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
					
					// Set size slightly smaller than fullscreen and center the window.
					int32 width = videoMode->width * 0.9;
					int32 height = videoMode->height * 0.9;
					int32 x = (videoMode->width - width) / 2.0;
					int32 y = (videoMode->height - height) / 2.0;
					
					glfwSetWindowMonitor(gWindow, NULL, x, y, width, height, 0);
					gFullscreen = false;
				}
				else {
					GLFWmonitor* monitor = glfwGetPrimaryMonitor();
					const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
					glfwSetWindowMonitor(gWindow, monitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
					gFullscreen = true;
				}
			}

			if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
			{
				FREE_CAM_ON = !FREE_CAM_ON;
				if (FREE_CAM_ON)
					std::cout << "Switched to Controlling FreeCam" << std::endl;
				else
					std::cout << "Switched to Controlling Player" << std::endl;
			}

			if (key == GLFW_KEY_F4 && action == GLFW_PRESS)
			{
				PLAYER_COLLISION_AABB = !PLAYER_COLLISION_AABB;

				gPlayer->setCollisionMode(PLAYER_COLLISION_AABB ? CollisionMode::AABB : CollisionMode::Sphere);

				if (PLAYER_COLLISION_AABB)
					std::cout << "Switched to Player Collision AABB testing" << std::endl;
				else
					std::cout << "Switched to Player Collision Sphere testing" << std::endl;
			}

			if (key == GLFW_KEY_HOME && action == GLFW_PRESS)
			{
				gPlayer->transform.xPos = playerPosition.x;
				gPlayer->transform.yPos = playerPosition.y;
				gPlayer->transform.zPos = playerPosition.z;
			}
		});

		// Close application when esc is pressed
		InputManager::instance()->registerKeyCallback([](int32 key, int32 action) {
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				glfwSetWindowShouldClose(gWindow, 1);
			}
		});

		RenderingContext::init();
		RenderingContext::get()->camera.setPerspective(45, SCREENWIDTH/(float32)SCREENHEIGHT);
		chunkShader = RenderingContext::get()->shaderCache.loadShaderProgram("chunk_shader", "chunk_vert.glsl", "chunk_frag.glsl");
		chunkTexture = RenderingContext::get()->textureCache.loadTexture2DArray("chunk_texture", 7, "tiles.png");

		// Load skybox texture
		CubeMapPaths paths;
		paths.bk = "MCLITEbk.tga";
		paths.ft = "MCLITEft.tga";
		paths.up = "MCLITEup.tga";
		paths.dn = "MCLITEdn.tga";
		paths.lf = "MCLITElf.tga";
		paths.rt = "MCLITErt.tga";
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

	// FreeCam
	gCameraController = new FreeCameraController(&RenderingContext::get()->camera);

	// Player
	gPlayer = new Player(&RenderingContext::get()->camera);
	gPlayer->transform.translateLocal(playerPosition.x, playerPosition.y, playerPosition.z);

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

	initTestCube();

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
	delete gPlayer;
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

	GLFWwindow* window = nullptr;

	if (gFullscreen) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
		SCREENWIDTH = videoMode->width;
		SCREENHEIGHT = videoMode->height;
		window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Final Project", monitor, nullptr);
	}
	else {
		window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Final Project", nullptr, nullptr);
	}

	if (!window) {
		glfwTerminate();
		throw std::runtime_error("Error: failed to initialize window.");
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int32 width, int32 height) -> void {
		
		// Width and height are 0 when we alt-tab while in fullscreen, I have no idea why it's done that way.
		if (width == 0 || height == 0) {
			return;
		}

		glViewport(0, 0, width, height);
		WaterRenderer::get()->resizeFBO(width, height);
		shadowMap->updateSize(width, height);
		SCREENWIDTH = width;
		SCREENHEIGHT = height;
		RenderingContext::get()->camera.setPerspective(45, SCREENWIDTH / (float32)SCREENHEIGHT);
	});

	return window;
}

void update(float32 deltaSeconds) {
	INSTRUMENT_FUNCTION("Update", Profiler::Color::Orchid);

	// Update logic...
	if(FREE_CAM_ON)
		gCameraController->update(deltaSeconds);
	else
		gPlayer->update(deltaSeconds);

	glm::vec3 currentChunk = ChunkManager::instance()->getCurrentChunk(gPlayer->getPosition());

	// Spooky hack lol
	static glm::vec3 lastChunk(currentChunk.x + 1.0f, currentChunk.y, currentChunk.z);

	if (currentChunk != lastChunk) {
		ChunkManager::instance()->loadChunks(currentChunk);
		lastChunk = currentChunk;
	}
	ChunkManager::instance()->uploadQueuedChunk();

	static glm::vec3 position;

	if (FREE_CAM_ON)
	{
		position = glm::vec3(RenderingContext::get()->camera.transform.xPos,
			RenderingContext::get()->camera.transform.yPos,
			RenderingContext::get()->camera.transform.zPos);
	}
	else
	{
		position = gPlayer->getPosition();
	}

	chunkShader->use();
	chunkShader->setUniform("viewPos", position);

	static float32 panner = 0;
	panner += 0.05 * deltaSeconds;
	RenderingContext::get()->shaderCache.getShaderProgram("water_shader")->use();
	RenderingContext::get()->shaderCache.getShaderProgram("water_shader")->setUniform("viewPos", position);
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

	//First Pass (Shadows)
	RenderingContext::get()->shaderCache.getShaderProgram("sm_shader")->use();
	shadowMap->updateMVP(lightDirection);
	shadowMap->bindForWriting();
	glClear(GL_DEPTH_BUFFER_BIT);
#ifdef RENDER_SHADOWS
	for (const auto& chunk : chunks) {
		glBindVertexArray(chunk.second.getVao());
		glDrawElementsInstanced(GL_TRIANGLES, cube::numIndices, GL_UNSIGNED_INT, nullptr, chunk.second.getBlockCount());
	}
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Cull chunks not visible to the camera
	std::vector<const Chunk*> visibleChunks;
	for (const auto& chunk : chunks) {
		if (RenderingContext::get()->camera.intersectsFrustum(chunk.second)) {
			visibleChunks.push_back(&chunk.second);
		}
	}
	//std::cout << visibleChunks.size() << std::endl;

	// Second Pass (render refraction texture)
	glBindFramebuffer(GL_FRAMEBUFFER, WaterRenderer::get()->getRefractionFBO());
	chunkShader->use();
	chunkShader->setUniform("vpMatrix", RenderingContext::get()->camera.getViewProjectionMatrix());
	chunkShader->setUniform("waterPlaneHeight", WaterRenderer::get()->getY());
	chunkShader->setUniform("lightSpaceMatrix", shadowMap->getMVP());
	chunkTexture->bind(Texture::UNIT_0);
	shadowMap->bindForReading();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifdef RENDER_WATER
	glEnable(GL_CLIP_DISTANCE0);
	for (const auto& chunk : visibleChunks) {
		glBindVertexArray(chunk->getVao());
		glDrawElementsInstanced(GL_TRIANGLES, cube::numIndices, GL_UNSIGNED_INT, nullptr, chunk->getBlockCount());
	}
	glDisable(GL_CLIP_DISTANCE0);
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Render chunks
	for (const auto& chunk : visibleChunks) {
		glBindVertexArray(chunk->getVao());
		glDrawElementsInstanced(GL_TRIANGLES, cube::numIndices, GL_UNSIGNED_INT, nullptr, chunk->getBlockCount());

#ifdef COMPILE_DRAW_NORMALS
		chunkNormalsShader->use();
		glDrawElementsInstanced(GL_POINTS, cube::numIndices, GL_UNSIGNED_INT, nullptr, chunk->getBlockCount());
		chunkShader->use();
#endif
	}

	// Render water
#ifdef RENDER_WATER
	WaterRenderer::get()->prepare();
	for (const auto& chunk : visibleChunks) {
		glm::vec3 pos = chunk->getPosition();

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
