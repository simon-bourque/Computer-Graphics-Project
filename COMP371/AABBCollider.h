#pragma once

#include <glm/glm.hpp>

/*
* Box is defined by point Max and point Min
* P represents the 'origin' of a voxel

			+Y		point Max
			|			*
			|
			|
			|
		   P|___________+X
		   /
		  /
	   +Z/
	point Min
*/

struct AABBCollider {
	AABBCollider(const glm::vec3& pointMin, const glm::vec3& pointMax)
		: pointMin(pointMin)
		, pointMax(pointMax)
	{}

	AABBCollider(const glm::vec3& bottomCorner, const float& l)
	{
		pointMin = glm::vec3(bottomCorner.x, bottomCorner.y, bottomCorner.z + l);
		pointMax = glm::vec3(bottomCorner.x + l, bottomCorner.y + l, bottomCorner.z);
	}

	static AABBCollider centeredOnPoint(const glm::vec3& center, const float& l)
	{
		int r = l / 2;
		glm::vec3 pMin = glm::vec3(center.x - r, center.y, center.z + r);
		glm::vec3 pMax = glm::vec3(center.x + r, center.y + r, center.z);

		return AABBCollider(pMin, pMax);
	}

	static bool checkCollision(const AABBCollider& box1, const AABBCollider& box2)
	{
		bool insideX = box1.pointMax.x >= box2.pointMin.x && box1.pointMin.x <= box2.pointMax.x;
		bool insideY = box1.pointMax.y >= box2.pointMin.y && box1.pointMin.y <= box2.pointMax.y;
		bool insideZ = box1.pointMax.z <= box2.pointMin.z && box1.pointMin.z >= box2.pointMax.z;

		return insideX && insideY && insideZ;
	}

	static bool checkPointInAABB(const glm::vec3& point, const AABBCollider& box)
	{
		bool insideX = point.x >= box.pointMin.x && point.x <= box.pointMax.x;
		bool insideY = point.y >= box.pointMin.y && point.y <= box.pointMax.y;
		bool insideZ = point.z <= box.pointMin.z && point.z >= box.pointMax.z;

		return insideX && insideY && insideZ;
	}

private:
	glm::vec3 pointMin;
	glm::vec3 pointMax;
};
