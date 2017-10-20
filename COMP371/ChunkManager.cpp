#include "ChunkManager.h"

#include <math.h>

ChunkManager::ChunkManager() 
{
	cmSemaphore = CreateSemaphore(
		NULL,	//Default security attributes
		0,		//Initial count
		4,		//Maximum count
		NULL	//Unnamed semaphore
	);

	for (uint32 i = 0; i < THREADCOUNT; i++)
	{
		cmThreadH[i] = CreateThread(
			NULL,			//Default security attributes
			0,				//Default stack size
			cmRoutine,		//Thread function name
			NULL,			//Argument to thread function
			0,				//Creation flag
			&cmThreadId[i]	//The thread identifiers
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
	WaitForSingleObject(ChunkManager::sChunkManager->getSemaphoreHandle(), INFINITE);

	uint32 queueSize = ChunkManager::sChunkManager->cmInQueue.size();
	for (uint32 i = 0; i < queueSize; i++)
	{
		Chunk tempChunk = Chunk(ChunkManager::sChunkManager->retrieveData());
		//Generate VAOs and VBOs and stuff
	}

	ChunkManager::sChunkManager->cmOutMutex.lock();
	//Push stuff to output queue
	ChunkManager::sChunkManager->cmOutMutex.unlock();

	return 0;
}

void ChunkManager::loadChunks(glm::vec3 playerPosition) 
{
	//Coordinates pointers to make the algorithm easier. Casting them to integer to get the coordinate of the chunk the player is standing in
	float32 currentX = floor((playerPosition.x / CHUNKWIDTH) + 0.5)*CHUNKWIDTH;
	float32 currentZ = floor((playerPosition.z / CHUNKWIDTH) + 0.5)*CHUNKWIDTH;
	std::vector<glm::vec3> chunksToLoad;
	uint32 decrementor = 0;

	//Chunk on player
	chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
	//Middle pass
	currentX = playerPosition.x, currentZ = playerPosition.z;
	for (uint32 i = 1; i <= LOADINGRADIUS; i++)
	{
		currentZ = i*CHUNKWIDTH;
		chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
		chunksToLoad.push_back(glm::vec3(currentX, 0, -currentZ));
	}
	//Top pass
	currentX = playerPosition.x;
	for (uint32 i = 1; i <= LOADINGRADIUS; i++) 
	{
		currentZ = playerPosition.z;
		currentX = playerPosition.x + i*CHUNKWIDTH;
		chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
		chunksToLoad.push_back(glm::vec3(-currentX, 0, currentZ));
		for (uint32 k = 1; k < LOADINGRADIUS - decrementor; k++) 
		{
			currentZ = playerPosition.z + k*CHUNKWIDTH;
			chunksToLoad.push_back(glm::vec3(currentX, 0, currentZ));
			chunksToLoad.push_back(glm::vec3(-currentX, 0, currentZ));
			chunksToLoad.push_back(glm::vec3(currentX, 0, -currentZ));
			chunksToLoad.push_back(glm::vec3(-currentX, 0, -currentZ));
		}
		decrementor++;
	}
	//Check for already loaded chunks and unload chunks if they are not going to be rendered
	for (auto it = chunksToLoad.begin(); it != chunksToLoad.end(); it++) 
	{
		for (uint32 j = 0; j < cmLoadedChunks.size(); j++) 
		{
			if (*it == cmLoadedChunks.at(j).getPosition()) 
			{
				chunksToLoad.erase(it);
			}
		}
	}
	for (auto it = cmLoadedChunks.begin(); it != cmLoadedChunks.end(); it++)
	{
		for (uint32 j = 0; j < chunksToLoad.size(); j++)
		{
			if (it->getPosition() == chunksToLoad.at(j))
			{
				cmLoadedChunks.erase(it);
			}
		}
	}
	loadData(chunksToLoad);
}

void ChunkManager::loadData(std::vector<glm::vec3> data)
{
	cmInMutex.lock();

	for (uint32_t i = 0; i < data.size(); i++)
	{
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
glm::vec3 ChunkManager::retrieveData()
{
	cmInMutex.lock();

	glm::vec3 data = cmInQueue.front();
	cmInQueue.pop();

	cmInMutex.unlock();

	return data;
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

ChunkManager* ChunkManager::sChunkManager;
