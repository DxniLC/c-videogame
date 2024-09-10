#pragma once

#include <glm/glm.hpp>

struct Plane
{

	glm::vec3 Normal{0.0f, 1.0f, 0.0f};

	// Origin to near plane point
	float DistanceOfOrigin{0.0f};

	Plane() = default;

	Plane(const glm::vec3 &p1, const glm::vec3 &norm)
		: Normal(glm::normalize(norm)),
		  DistanceOfOrigin(glm::dot(Normal, p1))
	{
	}

	// Restar posiciones para saber si el punto esta dentro o fuera del plano
	float getSignedDistanceToPlane(const glm::vec3 &point) const
	{
		// Negativo fuera, positivo dentro
		return glm::dot(Normal, point) - DistanceOfOrigin;
	}
};