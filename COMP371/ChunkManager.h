#pragma once
//Standard Library
#include <queue>
#include <mutex>

//Windows API
#include <Windows.h>

//External includes
#include "glm\glm.hpp"

//Local headers
#include "Types.h"

class ChunkManager {

public:
	ChunkManager();
	~ChunkManager();

	//Getters
	HANDLE getSemaphoreHandle();

	//Data Manipulation
	void loadData(std::vector<glm::vec3> data);
	glm::vec3 retrieveData();

	//Chunk Loading Routine
	friend DWORD WINAPI cmRoutine(LPVOID p);

	//Chunk Dimensions
	static const uint32 CHUNKWIDTH = 32;
	static const uint32 CHUNKLENGTH = 32;
	static const uint32 CHUNKHEIGHT = 256;
	static const uint32 NUMBEROFBLOCKS = CHUNKWIDTH*CHUNKLENGTH*CHUNKHEIGHT;

private:
	//Multi-Threading Variables
	static const uint32 THREADCOUNT = 4;
	HANDLE cmThreadH [THREADCOUNT];
	DWORD cmThreadId [THREADCOUNT];

	HANDLE cmSemaphore;
	std::mutex cmInMutex;
	std::mutex cmOutMutex;

	//Input and Output queues
	std::queue<glm::vec3> cmInQueue;
	std::queue<glm::vec3> cmOutQueue;

	//ChunkManager singleton
	static ChunkManager* sChunkManager;
};

