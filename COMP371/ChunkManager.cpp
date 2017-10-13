#include "ChunkManager.h"

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

//Getters
HANDLE ChunkManager::getSemaphoreHandle() {
	return cmSemaphore;
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
}

ChunkManager* ChunkManager::sChunkManager;
