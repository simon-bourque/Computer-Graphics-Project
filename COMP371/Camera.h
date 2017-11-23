#pragma once

#include "Transform.h"
#include "Types.h"

#include "Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>


class Camera {
public:
	Transform transform;
private:
	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewProjectionMatrix;

	// Frustum:
	// 0: top plane
	// 1: bottom plane
	// 2: left plane
	// 3: right plane
	// 4: near plane
	// 5: far plane
	glm::vec4 m_frustum[6];
	float32 m_near;
	float32 m_far;

	bool Camera::intersectPlane(const glm::vec4& plane, const glm::vec3& point);
public:
	Camera();
	Camera(float32 fov, float32 aspectRatio, float32 nearPlane = 0.1f, float32 farPlane = 1000.0f);
	virtual ~Camera();

	void updateViewProjectMatrix();

	glm::mat4 getViewProjectionMatrix() const { return m_viewProjectionMatrix; };
	glm::mat4 getProjectionMatrix() const { return m_projectionMatrix; };
	glm::mat4 getViewMatrix() const { return m_viewMatrix; };
	glm::vec3 getForward() const { return glm::rotate(transform.rotation, glm::vec3(0, 0, -1)); };
	glm::vec3 getUp() const { return glm::rotate(transform.rotation, glm::vec3(0, 1, 0)); };

	void setPerspective(float32 fov, float32 aspectRatio, float32 nearPlane = 0.1f, float32 farPlane = 1000.0f);

	bool intersectsFrustum(const Chunk& chunk);
};

