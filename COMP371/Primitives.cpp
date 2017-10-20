
#include "Types.h"
#include <vector>

namespace cube {
	extern const uint32 numVertices = 8;
	extern const float32 vertices[numVertices * 3] = {
		-0.5f, -0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,

		-0.5f, -0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, -0.5f
	};

	extern const uint32 numIndices = 36;
	extern const uint32 indices[numIndices] = {
		0,1,2,
		0,2,3,
		4,5,1,
		4,1,0,
		3,2,6,
		3,6,7,
		7,6,5,
		7,5,4,
		1,5,6,
		1,6,2,
		0,4,7,
		0,7,3
	};

	void fill(std::vector<float32>& vertices, std::vector<uint32>& indices) {
		for (int32 i = 0; i < (numVertices * 3); i++) {
			vertices.push_back(cube::vertices[i]);
		}

		for (int32 i = 0; i < numIndices; i++) {
			indices.push_back(cube::indices[i]);
		}
	}
}