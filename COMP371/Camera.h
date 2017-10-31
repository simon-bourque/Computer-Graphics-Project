#pragma once

#include "Transform.h"
#include "Types.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>


class Camera {
public:
	Transform transform;
private:
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewProjectionMatrix;
public:
	Camera();
	Camera(float32 fov, float32 aspectRatio, float32 near = 0.1f, float32 far = 1000.0f);
	virtual ~Camera();

	void updateViewProjectMatrix();

	glm::mat4 getViewProjectionMatrix() const { return m_viewProjectionMatrix; };
	glm::mat4 getViewMatrix() const;
	glm::vec3 getForward() const { return glm::rotate(transform.rotation, glm::vec3(0, 0, -1)); };
	glm::vec3 getUp() const { return glm::rotate(transform.rotation, glm::vec3(0, 1, 0)); };

	void setPerspective(float32 fov, float32 aspectRatio, float32 near, float32 far);
};

