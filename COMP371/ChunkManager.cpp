#include "ChunkManager.h"

#include <math.h>
#include <iostream>

#include <GL/glew.h>
#include "Primitives.h"

constexpr int64 encodePosition(int32 x, int32 z)
{
	return ((int64)x << 32) | ((int64)z & 0x00000000FFFFFFFF);
}

ChunkManager::ChunkManager()
	:cmTerrainBuilder(ChunkManager::SEED)
{
	cmSemaphore = CreateSemaphore(
		NULL,
		0,
		9999,
		NULL
	);

	for (uint32 i = 0; i < THREADCOUNT; i++)
	{
		cmThreadH[i] = CreateThread(
			NULL,				//Default security attributes
			0,					//Default stack size
			cmRoutine,			//Thread function name
			NULL,				//Argument to thread function
			CREATE_SUSPENDED,	//Creation flag
			&cmThreadId[i]		//The thread identifiers
		);
	}

	if (cmSemaphore == NULL)
	{
		printf("CreateSemaphore error: %d\n", GetLastError());
		//ERROR LOGGING
	}
}

//Chunk Loading Thread Routine
DWORD WINAPI cmRoutine(LPVOID p)
{
	for(;;)
	{
		WaitForSingleObject(ChunkManager::sChunkManager->getSemaphoreHandle(), INFINITE);
		Chunk tempChunk = Chunk(ChunkManager::sChunkManager->fetchQueueIn());
		std::vector<Block> newBlocks = ChunkManager::sChunkManager->cmTerrainBuilder.getChunkBlocks(tempChunk);
		ChunkManager::sChunkManager->pushQueueOut(tempChunk.getPosition(), newBlocks);
	}
	return 0;
}

void ChunkManager::loadChunks(glm::vec3 currentChunk) 
{
	float32 flooredX = currentChunk.x, flooredZ = currentChunk.z;
	float32 currentX = flooredX, currentZ = flooredZ;
	std::vector<glm::vec3> chunksToLoad;
	uint32 decrementor = 0;

	//Chunk on player
	chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
	//Middle pass
	for (uint32 i = 1; i <= LOADINGRADIUS; i++)
	{
		currentZ = flooredZ + i*CHUNKWIDTH;
		chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
		currentZ = flooredZ - i*CHUNKWIDTH;
		chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
	}
	//Outer pass
	for (uint32 i = 1; i <= LOADINGRADIUS; i++) 
	{
		currentX = flooredX + i*CHUNKWIDTH;
		currentZ = flooredZ;
		chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
		currentX = flooredX - i*CHUNKWIDTH;
		chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
		for (uint32 k = 1; k < LOADINGRADIUS - decrementor; k++) 
		{
			currentZ = flooredZ + k*CHUNKWIDTH;
			chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
			currentX = flooredX + i*CHUNKWIDTH;
			chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
			currentZ = flooredZ - k*CHUNKWIDTH;
			chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
			currentX = flooredX - i*CHUNKWIDTH;
			chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
		}
		decrementor++;
	}

	//Check for loaded and loading chunks
	for (int32 i = 0; i < chunksToLoad.size(); i++)
	{
		int64 chunkPosition = encodePosition(chunksToLoad.at(i).x, chunksToLoad.at(i).z);

		const auto& loadedIt = cmLoadedChunks.find(chunkPosition);
		const auto& loadingIt = cmLoadingChunks.find(chunkPosition);

		if (loadedIt != cmLoadedChunks.end() || loadingIt != cmLoadingChunks.end())
		{
			chunksToLoad.erase(chunksToLoad.begin() + i);
		}
	}
	pushQueueIn(chunksToLoad);
}

void ChunkManager::pushQueueIn(std::vector<glm::vec3> data)
{
	cmInMutex.lock();

	for (uint32_t i = 0; i < data.size(); i++)
	{
		glm::vec3 currentPos = data.at(i);
		cmLoadingChunks[encodePosition(currentPos.x, currentPos.z)] = Chunk(currentPos);
		cmInQueue.push(currentPos);

		//Signal semaphore
		ReleaseSemaphore(
			cmSemaphore,
			1,
			NULL
		);
	}

	cmInMutex.unlock();
}
void ChunkManager::pushQueueOut(const glm::vec3 chunkPosition, std::vector<Block>& data)
{
	cmOutMutex.lock();
	
	cmOutQueue.push(std::pair<glm::vec3, std::vector<Block>>(chunkPosition, data));

	cmOutMutex.unlock();
}
glm::vec3 ChunkManager::fetchQueueIn()
{
	cmInMutex.lock();

	glm::vec3 data = cmInQueue.front();
	cmInQueue.pop();

	cmInMutex.unlock();

	return data;
}

void ChunkManager::uploadQueuedChunk()
{
	cmOutMutex.lock();
	
	if (cmOutQueue.empty()) {
		cmOutMutex.unlock();
		return;
	}

	const auto data = cmOutQueue.front();
	cmOutQueue.pop();
	cmOutMutex.unlock();

	uploadChunk(data.first, data.second);
}

ChunkManager* ChunkManager::instance() 
{
	if (!sChunkManager)
	{
		sChunkManager = new ChunkManager;
		sChunkManager->startThreads();
	}
	return sChunkManager;
}

void ChunkManager::startThreads()
{
	for (uint32 i = 0; i < THREADCOUNT; i++)
	{
		ResumeThread(cmThreadH[i]);
	}
}

glm::vec3 ChunkManager::getCurrentChunk(glm::vec3 playerPosition) const
{
	float32 flooredX = floor((playerPosition.x / CHUNKWIDTH) + 0.5)*CHUNKWIDTH;
	float32 flooredZ = floor((playerPosition.z / CHUNKWIDTH) + 0.5)*CHUNKWIDTH;

	return glm::vec3(flooredX, 0, flooredZ);
}

ChunkManager::~ChunkManager()
{
	CloseHandle(cmSemaphore);

	for (uint32_t i = 0; i < THREADCOUNT; i++)
	{
		CloseHandle(cmThreadH[i]);
	}

	delete sChunkManager;
}

void ChunkManager::uploadChunk(const glm::vec3& chunkPosition, const std::vector<Block>& chunkData) {
	GLuint chunkVao;
	glGenVertexArrays(1, &chunkVao);
	glBindVertexArray(chunkVao);

	std::vector<float32> vertices;
	std::vector<float32> uvCoords;
	std::vector<float32> normals;
	std::vector<uint32> indices;
	std::vector<GLuint> vbos;

	cube::fill(vertices, uvCoords, normals, indices);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float32) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
	vbos.push_back(vbo);

	GLuint uvVbo;
	glGenBuffers(1, &uvVbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float32) * uvCoords.size(), uvCoords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, nullptr);
	vbos.push_back(uvVbo);

	GLuint normalVbo;
	glGenBuffers(1, &normalVbo);
	glBindBuffer(GL_ARRAY_BUFFER, normalVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float32) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, nullptr);
	vbos.push_back(normalVbo);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * indices.size(), indices.data(), GL_STATIC_DRAW);
	vbos.push_back(ebo);

	GLuint positionBuffer;
	GLuint textureIndexBuffer;
	glGenBuffers(1, &positionBuffer);
	glGenBuffers(1, &textureIndexBuffer);

	float32* positions = new float32[chunkData.size() * 3];
	uint32* textureIndices = new uint32[chunkData.size()];

	for (int32 i = 0; i < chunkData.size(); i++) {
		glm::vec3 pos = chunkData[i].getPosition();
		uint32 type = static_cast<uint32>(chunkData[i].getBlockType());

		positions[(i * 3)] = pos.x;
		positions[(i * 3) + 1] = pos.y;
		positions[(i * 3) + 2] = pos.z;
		textureIndices[i] = type;
	}

	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float32) * chunkData.size() * 3, positions, GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, false, 0, nullptr);
	glVertexAttribDivisor(3, 1);

	glBindBuffer(GL_ARRAY_BUFFER, textureIndexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint32) * chunkData.size(), textureIndices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, 0, nullptr);
	glVertexAttribDivisor(4, 1);

	vbos.push_back(positionBuffer);
	vbos.push_back(textureIndexBuffer);

	delete[] positions;
	delete[] textureIndices;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Chunk chunky(chunkPosition);
	chunky.setVao(chunkVao);
	chunky.setVbos(vbos);
	chunky.setBlockCount(chunkData.size());
	cmLoadedChunks[encodePosition(chunkPosition.x, chunkPosition.z)] = chunky;
	cmLoadingChunks.erase(encodePosition(chunkPosition.x, chunkPosition.z));
}

ChunkManager* ChunkManager::sChunkManager = nullptr;
