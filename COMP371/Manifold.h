#pragma once

#include "Types.h"
#include <glm/glm.hpp>

struct Manifold {
	glm::vec3 seperationVector;
	float32 depth; // Not used atm
};

