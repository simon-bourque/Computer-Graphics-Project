#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() : Camera(45.0f, 16.0f/9.0f) {}

Camera::Camera(float32 fov, float32 aspectRatio, float32 near, float32 far) {
	setPerspective(fov, aspectRatio, near, far);
}

Camera::~Camera() {}

void Camera::updateViewProjectMatrix() {
	glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(-transform.xPos, -transform.yPos, -transform.zPos));
	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1 / transform.xScale, 1 / transform.yScale, 1 / transform.zScale));

	m_viewProjectionMatrix = m_projectionMatrix * glm::toMat4(glm::conjugate(transform.rotation)) * s * t;
}

void Camera::setPerspective(float32 fov, float32 aspectRatio, float32 near, float32 far) {
	m_projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, near, far);
}
