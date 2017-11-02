#pragma once
//Standard Library
#include <queue>
#include <unordered_map>
#include <mutex>

//Windows API
#include <Windows.h>

//External includes
#include "glm\glm.hpp"

//Local headers
#include "TerrainBuilder.h"
#include "Types.h"
#include "Block.h"
#include "Chunk.h"

#include "Block.h"

class ChunkManager
{
public:
	ChunkManager();
	~ChunkManager();

	//Getters
	static ChunkManager* instance();
	HANDLE getSemaphoreHandle() const { return cmSemaphore; }
	glm::vec3 getCurrentChunk(glm::vec3 playerPosition) const;

	//Data Manipulation
	void loadChunks(glm::vec3 playerPosition);
	void pushQueueIn(std::vector<glm::vec3> data);
	void pushQueueOut(const glm::vec3 chunkPosition, std::vector<Block>& data);
	glm::vec3 fetchQueueIn();
	void uploadQueuedChunk();

	//Chunk Loading Routine
	friend DWORD WINAPI cmRoutine(LPVOID p);

	//Chunk Dimensions
	static const uint32 CHUNKWIDTH = 32;
	static const uint32 CHUNKHEIGHT = 256;
	static const uint32 NUMBEROFBLOCKS = CHUNKWIDTH*CHUNKWIDTH*CHUNKHEIGHT;

	const std::unordered_map<int64, Chunk>& getCurrentlyLoadedChunks() const { return cmLoadedChunks; };

private:
	static const uint32 SEED = 666;
	TerrainBuilder cmTerrainBuilder;

	//Multi-Threading Variables
	static const uint32 THREADCOUNT = 4;
	HANDLE cmThreadH [THREADCOUNT];
	DWORD cmThreadId [THREADCOUNT];
	void startThreads();

	HANDLE cmSemaphore;
	std::mutex cmInMutex;
	std::mutex cmOutMutex;

	//Loading Chunks
	static const uint32 LOADINGRADIUS = 4;
	std::unordered_map<int64, Chunk> cmLoadingChunks;
	std::unordered_map<int64, Chunk> cmLoadedChunks;

	//Input and Output queues
	std::queue<glm::vec3> cmInQueue;
	std::queue<std::pair<glm::vec3, std::vector<Block>>> cmOutQueue;

	//ChunkManager singleton
	static ChunkManager* sChunkManager;

	void uploadChunk(const glm::vec3& chunkPosition, const std::vector<Block>& chunkData);
	void unloadChunks(const glm::vec3& currentChunkPos);
};

