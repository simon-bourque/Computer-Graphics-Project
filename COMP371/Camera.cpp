#include "Camera.h"

#include "ChunkManager.h"

#include "Profiling.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() : Camera(45.0f, 16.0f/9.0f) {}

Camera::Camera(float32 fov, float32 aspectRatio, float32 nearPlane, float32 farPlane) {
	setPerspective(fov, aspectRatio, nearPlane, farPlane);
}

Camera::~Camera() {}

void Camera::updateViewProjectMatrix() {
	glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(-transform.xPos, -transform.yPos, -transform.zPos));
	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1 / transform.xScale, 1 / transform.yScale, 1 / transform.zScale));
	m_viewMatrix = glm::toMat4(glm::conjugate(transform.rotation)) * s * t;
	m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
}

void Camera::setPerspective(float32 fov, float32 aspectRatio, float32 nearPlane, float32 farPlane) {
	m_projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
	m_near = nearPlane;
	m_far = farPlane;
	m_aspectRatio = aspectRatio;

	// Setup frustum planes
	//float32 nearHalfHeight = nearPlane * tan(fov / 2.0f);
	//float32 nearHeight = nearHalfHeight * 2;
	//float32 nearWidth = aspectRatio * nearHeight;
	//float32 nearHalfWidth = nearWidth / 2.0f;
	
	float32 farHalfHeight = farPlane * tan(fov / 2.0f);
	float32 farHeight = farHalfHeight * 2;
	float32 farWidth = aspectRatio * farHeight;
	float32 farHalfWidth = farWidth / 2.0f;

	//glm::vec3 nearTopRight(nearHalfWidth, nearHalfHeight, nearPlane);
	//glm::vec3 nearTopLeft(-nearHalfWidth, nearHalfHeight, nearPlane);
	//glm::vec3 nearBottomRight(nearHalfWidth, -nearHalfHeight, nearPlane);
	//glm::vec3 nearBottomLeft(-nearHalfWidth, -nearHalfHeight, nearPlane);

	glm::vec3 farTopRight(farHalfWidth, farHalfHeight, farPlane);
	glm::vec3 farTopLeft(-farHalfWidth, farHalfHeight, farPlane);
	glm::vec3 farBottomRight(farHalfWidth, -farHalfHeight, farPlane);
	glm::vec3 farBottomLeft(-farHalfWidth, -farHalfHeight, farPlane);
	
	glm::vec3 topNormal(glm::normalize(glm::cross(farTopRight, farTopLeft)));
	glm::vec3 bottomNormal(glm::normalize(glm::cross(farBottomLeft, farBottomRight)));
	glm::vec3 leftNormal(glm::normalize(glm::cross(farTopLeft, farBottomLeft)));
	glm::vec3 rightNormal(glm::normalize(glm::cross(farBottomRight, farTopRight)));

	m_frustum[0] = glm::vec4(topNormal, 0.0f);
	m_frustum[1] = glm::vec4(bottomNormal, 0.0f);
	m_frustum[2] = glm::vec4(leftNormal, 0.0f);
	m_frustum[3] = glm::vec4(rightNormal, 0.0f);
	m_frustum[4] = glm::vec4(glm::vec3(0, 0, 1), nearPlane);
	m_frustum[5] = glm::vec4(glm::vec3(0, 0, -1), -farPlane);
}

std::vector<glm::vec4> Camera::getFrustumCorners(float32 fov, float32 aspectRatio) const
{
	// Setup frustum planes
	float32 nearHalfHeight = m_near * glm::tan(glm::radians(fov*aspectRatio / 2.0f));
	float32 nearHalfWidth = m_near * glm::tan(glm::radians(fov / 2.0f));


	float farPlane = ChunkManager::LOADINGRADIUS*ChunkManager::CHUNKWIDTH;
	float32 farHalfHeight = farPlane * glm::tan(glm::radians(fov*aspectRatio / 2.0f));
	float32 farHalfWidth = farPlane * glm::tan(glm::radians(fov / 2.0f));

	glm::vec3 nearTopRight(nearHalfWidth, nearHalfHeight, m_near);
	glm::vec3 nearTopLeft(-nearHalfWidth, nearHalfHeight, m_near);
	glm::vec3 nearBottomRight(nearHalfWidth, -nearHalfHeight, m_near);
	glm::vec3 nearBottomLeft(-nearHalfWidth, -nearHalfHeight, m_near);

	glm::vec3 farTopRight(farHalfWidth, farHalfHeight, farPlane);
	glm::vec3 farTopLeft(-farHalfWidth, farHalfHeight, farPlane);
	glm::vec3 farBottomRight(farHalfWidth, -farHalfHeight, farPlane);
	glm::vec3 farBottomLeft(-farHalfWidth, -farHalfHeight, farPlane);

	std::vector<glm::vec4>  frustumCorners;

	frustumCorners.push_back(glm::vec4(nearBottomLeft, 1.0f));
	frustumCorners.push_back(glm::vec4(nearBottomRight, 1.0f));
	frustumCorners.push_back(glm::vec4(nearTopLeft, 1.0f));
	frustumCorners.push_back(glm::vec4(nearTopRight, 1.0f));
	frustumCorners.push_back(glm::vec4(farBottomLeft, 1.0f));
	frustumCorners.push_back(glm::vec4(farBottomRight, 1.0f));
	frustumCorners.push_back(glm::vec4(farTopLeft, 1.0f));
	frustumCorners.push_back(glm::vec4(farTopRight, 1.0f));

	return frustumCorners;
}

bool Camera::intersectsFrustum(const Chunk& chunk) {
	INSTRUMENT_FUNCTION("FrustumCulling", Profiler::Color::Crimson);

	glm::vec3 pos = chunk.getPosition();
	float32 chunkHalfWidth = ChunkManager::CHUNKWIDTH / 2.0f;
	float32 chunkHalfHeight = ChunkManager::CHUNKHEIGHT / 2.0f;

	glm::vec3 points[8];
	points[0] = glm::vec3(pos.x - chunkHalfWidth, ChunkManager::CHUNKHEIGHT, pos.z - chunkHalfWidth); // Near Top Left
	points[1] = glm::vec3(pos.x + chunkHalfWidth, ChunkManager::CHUNKHEIGHT, pos.z - chunkHalfWidth); // Near Top Right
	points[2] = glm::vec3(pos.x - chunkHalfWidth, -((int32)ChunkManager::CHUNKHEIGHT), pos.z - chunkHalfWidth); // Near Bottom Left
	points[3] = glm::vec3(pos.x + chunkHalfWidth, -((int32)ChunkManager::CHUNKHEIGHT), pos.z - chunkHalfWidth); // Near Bottom Right

	points[4] = glm::vec3(pos.x - chunkHalfWidth, ChunkManager::CHUNKHEIGHT, pos.z + chunkHalfWidth); // Far Top Left
	points[5] = glm::vec3(pos.x + chunkHalfWidth, ChunkManager::CHUNKHEIGHT, pos.z + chunkHalfWidth); // Far Top Right
	points[6] = glm::vec3(pos.x - chunkHalfWidth, -((int32)ChunkManager::CHUNKHEIGHT), pos.z + chunkHalfWidth); // Far Bottom Left
	points[7] = glm::vec3(pos.x + chunkHalfWidth, -((int32)ChunkManager::CHUNKHEIGHT), pos.z + chunkHalfWidth); // Far Bottom Right

	for (int32 plane = 0; plane < 6; plane++) {
		
		bool pointInFrustum = false;
		for (int32 pointIndex = 0; pointIndex < 8; pointIndex++) {
			glm::vec3 point = m_viewMatrix * glm::vec4(points[pointIndex], 1.0f);

			if (intersectPlane(m_frustum[plane], point)) {
				pointInFrustum = true;
				break;
			}
		}

		// If all points are on the positive side of the plane then there is no intersection
		if (!pointInFrustum) {
			return false;
		}
	}

	return true;
}


bool Camera::intersectPlane(const glm::vec4& plane, const glm::vec3& point) {
	float32 result = (plane.x * point.x) + (plane.y * point.y) + (plane.z * point.z) + plane.w;
	return (result <= 0);
}
