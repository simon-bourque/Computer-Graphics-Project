#pragma once

#include "Types.h"

namespace cube {
	extern const uint32 numVertices;
	extern const float32 vertices[];

	extern const uint32 numUvCoords;
	extern const float32 uvCoords[];

	extern const uint32 numNormals;
	extern const float32 normals[];

	extern const uint32 numIndices;
	extern const uint32 indices[];

	void fill(std::vector<float32>& vertices, std::vector<float32>& uvCoords, std::vector<float32>& normals, std::vector<uint32>& indices);
}