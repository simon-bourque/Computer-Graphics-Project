#pragma once

#include <glm/glm.hpp>

struct SphereCollider {

	SphereCollider(const glm::vec3& center, const float& radius)
	{
		m_center = center;
		m_radius = radius;
	}

	static bool checkCollision(const SphereCollider& sphere1, const SphereCollider& sphere2)
	{
		float distance, radiiSum;

		distance = glm::length(sphere2.m_center - sphere1.m_center);
		radiiSum = sphere1.m_radius + sphere2.m_radius;

		return (distance < radiiSum);
	}

	static SphereCollider centeredOnVoxel(const glm::vec3& cornerPoint)
	{
		float r = 0.5f;
		
		glm::vec3 centeredPoint(cornerPoint.x + r, cornerPoint.y + r, cornerPoint.z + r);

		return SphereCollider(centeredPoint, r);
	}

private:
	glm::vec3 m_center;
	float m_radius;
};
