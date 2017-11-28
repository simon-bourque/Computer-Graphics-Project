#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Transform::Transform() : Transform(0, 0, 0) {}
Transform::Transform(float32 xPos, float32 yPos, float32 zPos) : xPos(xPos), yPos(yPos), zPos(zPos), xScale(1), yScale(1), zScale(1) {}
Transform::~Transform() {}

void Transform::translate(float32 x, float32 y, float32 z) {
	xPos += x;
	yPos += y;
	zPos += z;
}

void Transform::translate(const glm::vec3& displacement) {
	translate(displacement.x, displacement.y, displacement.z);
}

void Transform::translateLocal(float32 x, float32 y, float32 z) {
	glm::vec3 delta = glm::rotate(rotation, glm::vec3(x, y, z));

	translate(delta.x, delta.y, delta.z);
}

void Transform::rotate(float32 pitch, float32 yaw, float32 roll) {
	glm::quat rotPitch = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
	glm::quat rotYaw = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
	glm::quat rotRoll = glm::angleAxis(roll, glm::vec3(0, 0, 1));
	rotation = rotRoll * rotPitch * rotYaw * rotation;
}

void Transform::rotateLocal(float32 pitch, float32 yaw, float32 roll) {
	glm::vec3 localXAxis = glm::rotate(rotation, glm::vec3(1, 0, 0));
	glm::vec3 localYAxis = glm::rotate(rotation, glm::vec3(0, 1, 0));
	glm::vec3 localZAxis = glm::rotate(rotation, glm::vec3(0, 0, 1));

	glm::quat rotPitch = glm::angleAxis(pitch, localXAxis);
	glm::quat rotYaw = glm::angleAxis(yaw, localYAxis);
	glm::quat rotRoll = glm::angleAxis(roll, localZAxis);
	rotation = rotRoll * rotPitch * rotYaw * rotation;
}

void Transform::orient(float32 pitch, float32 yaw, float32 roll) {
	glm::quat rotPitch = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
	glm::quat rotYaw = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
	glm::quat rotRoll = glm::angleAxis(roll, glm::vec3(0, 0, 1));
	rotation = rotRoll * rotPitch * rotYaw;
}

void Transform::scale(float32 s) {
	xScale *= s;
	yScale *= s;
	zScale *= s;
}

glm::mat4 Transform::getModelMatrix() const {
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, yPos, zPos));
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(xScale, yScale, zScale));
	
	return translation * scale * glm::toMat4(rotation);
}