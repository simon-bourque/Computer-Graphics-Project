#include "ChunkManager.h"

#include <math.h>

ChunkManager::ChunkManager() {
	cmSemaphore = CreateSemaphore(
		NULL,	//Default security attributes
		0,		//Initial count
		4,		//Maximum count
		NULL	//Unnamed semaphore
	);

	for (uint32 i = 0; i < THREADCOUNT; i++) {
		cmThreadH[i] = CreateThread(
			NULL,			//Default security attributes
			0,				//Default stack size
			cmRoutine,		//Thread function name
			NULL,			//Argument to thread function
			0,				//Creation flag
			&cmThreadId[i]	//The thread identifiers
		);
	}

	if (cmSemaphore == NULL) {
		printf("CreateSemaphore error: %d\n", GetLastError());
		//ERROR LOGGING
	}
}

void ChunkManager::loadChunks(glm::vec3 playerPosition) {
	//Coordinates pointers to make the algorithm easier. Casting them to integer to get the coordinate of the chunk the player is standing in
	float32 currentX = floor((playerPosition.x / CHUNKWIDTH) + 0.5)*CHUNKWIDTH;
	float32 currentZ = floor((playerPosition.z / CHUNKWIDTH) + 0.5)*CHUNKWIDTH;

	//Vector of chunks to then send to the input queue
	std::vector<glm::vec3> chunksToLoad;
	//Used to create the circular shape
	uint32 decrementor = 0;

	//Chunk on player
	chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
	//Middle pass
	currentX = playerPosition.x, currentZ = playerPosition.z;
	for (uint32 i = 1; i <= LOADINGRADIUS; i++) {
		currentZ = i*CHUNKWIDTH;
		chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
		chunksToLoad.push_back(glm::vec3(currentX, 0, -currentZ));
	}
	//Top pass
	currentX = playerPosition.x;
	for (uint32 i = 1; i <= LOADINGRADIUS; i++) {
		currentZ = playerPosition.z;
		currentX = playerPosition.x + i*CHUNKWIDTH;
		chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
		chunksToLoad.push_back(glm::vec3(-currentX, 0, currentZ));
		for (uint32 k = 1; k < LOADINGRADIUS - decrementor; k++) {
			currentZ = playerPosition.z + k*CHUNKWIDTH;
			chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
			chunksToLoad.push_back(glm::vec3(-currentX, 0, currentZ));
			chunksToLoad.push_back(glm::vec3(currentX, 0, -currentZ));
			chunksToLoad.push_back(glm::vec3(-currentX, 0, -currentZ));
		}
		decrementor++;
	}
	loadData(chunksToLoad);
}

void ChunkManager::loadData(std::vector<glm::vec3> data) {
	cmInMutex.lock();

	for (uint32_t i = 0; i < data.size(); i++) {
		cmInQueue.push(data.at(i));
	}

	cmInMutex.unlock();

	//Signal semaphore
	ReleaseSemaphore(
		cmSemaphore,
		1,
		NULL
	);
}
glm::vec3 ChunkManager::retrieveData() {
	cmOutMutex.lock();

	glm::vec3 data = cmOutQueue.front();
	cmOutQueue.pop();

	cmOutMutex.unlock();

	return data;
}

//Chunk Loading Thread Routine
DWORD WINAPI cmRoutine(LPVOID p) {
	WaitForSingleObject(ChunkManager::sChunkManager->getSemaphoreHandle(), INFINITE);

	ChunkManager::sChunkManager->cmOutMutex.lock();

	//Compute stuff

	ChunkManager::sChunkManager->cmOutMutex.unlock();

	return 0;
}

ChunkManager::~ChunkManager() {
	CloseHandle(cmSemaphore);

	for (uint32_t i = 0; i < THREADCOUNT; i++) {
		CloseHandle(cmThreadH[i]);
	}

	delete sChunkManager;
}

ChunkManager* ChunkManager::sChunkManager;
