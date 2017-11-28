#pragma once

#include "Types.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
	float32 xPos;
	float32 yPos;
	float32 zPos;

	glm::quat rotation;

	float32 xScale;
	float32 yScale;
	float32 zScale;

	Transform();
	Transform(float32 xPos, float32 yPos, float32 zPos);
	virtual ~Transform();

	void translate(float32 x, float32 y, float32 z);
	void translate(const glm::vec3& displacement);
	void translateLocal(float32 x, float32 y, float32 z);
	void rotate(float32 pitch, float32 yaw, float32 roll);
	void rotateLocal(float32 pitch, float32 yaw, float32 roll);
	void orient(float32 pitch, float32 yaw, float32 roll);
	void scale(float32 s);

	glm::mat4 getModelMatrix() const;
	glm::vec3 getPosition() const { return glm::vec3(xPos, yPos, zPos); }
};

