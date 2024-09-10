

#pragma once

#include <glm/mat4x4.hpp>

struct BoneInfo
{
	// bone id
	int id;
	// offset matrix transforms vertex from model space to bone space
	glm::mat4 offset;
};