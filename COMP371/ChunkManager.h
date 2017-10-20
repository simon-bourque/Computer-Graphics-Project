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
#include "Chunk.h"

class ChunkManager {

public:
	ChunkManager();
	~ChunkManager();

	//Getters
	static ChunkManager* instance() { return sChunkManager; }
	HANDLE getSemaphoreHandle() { return cmSemaphore; }

	//Data Manipulation
	void loadChunks(glm::vec3 playerPosition);
	void loadData(std::vector<glm::vec3> data);
	Chunk retrieveData();

	//Chunk Loading Routine
	friend DWORD WINAPI cmRoutine(LPVOID p);

	//Chunk Dimensions
	static const uint32 CHUNKWIDTH = 32;
	static const uint32 CHUNKHEIGHT = 256;
	static const uint32 NUMBEROFBLOCKS = CHUNKWIDTH*CHUNKWIDTH*CHUNKHEIGHT;

private:
	//Multi-Threading Variables
	static const uint32 THREADCOUNT = 4;
	HANDLE cmThreadH [THREADCOUNT];
	DWORD cmThreadId [THREADCOUNT];

	HANDLE cmSemaphore;
	std::mutex cmInMutex;
	std::mutex cmOutMutex;

	//Loaded chunks
	static const uint32 LOADINGRADIUS = 4;
	std::vector<Chunk> cmLoadedChunks;

	//Input and Output queues
	std::queue<glm::vec3> cmInQueue;
	std::queue<Chunk> cmOutQueue;

	//ChunkManager singleton
	static ChunkManager* sChunkManager;
};

