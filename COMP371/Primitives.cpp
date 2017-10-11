
#include "Types.h"

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
}