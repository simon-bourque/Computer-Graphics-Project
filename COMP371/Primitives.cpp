
#include "Types.h"
#include <vector>

namespace cube {
	extern const uint32 numVertices = 24;
	extern const float32 vertices[numVertices * 3] = {
		-0.5f, -0.5f, 0.5f, // Front
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,

		-0.5f, 0.5f, 0.5f, // Top
		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,

		-0.5f, -0.5f, -0.5f, // Back
		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f, 0.5f, // Bottom
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,

		-0.5f, -0.5f, 0.5f, // Left
		-0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		0.5f, -0.5f, 0.5f, // Right
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, -0.5f

	};

	extern const uint32 numUvCoords = 24;
	extern const float32 uvCoords[numUvCoords * 2] = {
		1.0f,1.0f,
		1.0f,0.0f,
		0.0f,0.0f,
		0.0f,1.0f,

		1.0f,1.0f,
		1.0f,0.0f,
		0.0f,0.0f,
		0.0f,1.0f,
		
		1.0f,1.0f,
		1.0f,0.0f,
		0.0f,0.0f,
		0.0f,1.0f,
		
		1.0f,1.0f,
		1.0f,0.0f,
		0.0f,0.0f,
		0.0f,1.0f,

		1.0f,1.0f,
		1.0f,0.0f,
		0.0f,0.0f,
		0.0f,1.0f,

		1.0f,1.0f,
		1.0f,0.0f,
		0.0f,0.0f,
		0.0f,1.0f,
		
	};

	extern const uint32 numNormals = 24;
	extern const float32 normals[numNormals * 3] = {
		0.0f,0.0f,1.0f, // Front
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,

		0.0f,1.0f,0.0f, // Top
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,

		0.0f,0.0f,-1.0f, // Back
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,

		0.0f,-1.0f,0.0f, // Bottom
		0.0f,-1.0f,0.0f,
		0.0f,-1.0f,0.0f,
		0.0f,-1.0f,0.0f,

		1.0f,0.0f,0.0f, // Left
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,

		-1.0f,0.0f,0.0f, // Right
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
	};

	extern const uint32 numIndices = 36;
	extern const uint32 indices[numIndices] = {
		0,1,2, // Front
		0,2,3,

		4,5,6, // Top
		4,6,7,

		8,9,10, // Back
		8,10,11,

		12,13,14, // Bottom
		12,14,15,

		16,17,18, // Left
		16,18,19,

		20,21,22, // Right
		20,22,23
	};

	void fill(std::vector<float32>& vertices, std::vector<float32>& uvCoords, std::vector<float32>& normals, std::vector<uint32>& indices) {
		for (int32 i = 0; i < (numVertices * 3); i++) {
			vertices.push_back(cube::vertices[i]);
		}

		for (int32 i = 0; i < (numUvCoords * 2); i++) {
			uvCoords.push_back(cube::uvCoords[i]);
		}

		for (int32 i = 0; i < (numNormals * 3); i++) {
			normals.push_back(cube::normals[i]);
		}

		for (int32 i = 0; i < numIndices; i++) {
			indices.push_back(cube::indices[i]);
		}
	}
}