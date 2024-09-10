#pragma once

#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct AssimpGLMHelper
{

	inline static glm::mat4 aiMatrixToGLM(const aiMatrix4x4 &aiMatrix)
	{
		glm::mat4 result;
		// the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		result[0][0] = aiMatrix.a1;
		result[1][0] = aiMatrix.a2;
		result[2][0] = aiMatrix.a3;
		result[3][0] = aiMatrix.a4;

		result[0][1] = aiMatrix.b1;
		result[1][1] = aiMatrix.b2;
		result[2][1] = aiMatrix.b3;
		result[3][1] = aiMatrix.b4;

		result[0][2] = aiMatrix.c1;
		result[1][2] = aiMatrix.c2;
		result[2][2] = aiMatrix.c3;
		result[3][2] = aiMatrix.c4;

		result[0][3] = aiMatrix.d1;
		result[1][3] = aiMatrix.d2;
		result[2][3] = aiMatrix.d3;
		result[3][3] = aiMatrix.d4;

		return result;
	}

	inline static glm::vec2 aiVector2ToGLM(const aiVector2D &aiVec)
	{
		return glm::vec2(aiVec.x, aiVec.y);
	}

	inline static glm::vec2 aiVector2ToGLM(const aiVector3D &aiVec)
	{
		return glm::vec2(aiVec.x, aiVec.y);
	}

	inline static glm::vec3 aiVector3ToGLM(const aiVector3D &aiVec)
	{
		return glm::vec3(aiVec.x, aiVec.y, aiVec.z);
	}

	inline static glm::vec4 aiColor4ToGLM(const aiColor4D &aiColor)
	{
		return glm::vec4(aiColor.r, aiColor.g, aiColor.b, aiColor.a);
	}

	inline static glm::quat aiQuatToGLM(const aiQuaternion &aiQuat)
	{
		return glm::quat(aiQuat.w, aiQuat.x, aiQuat.y, aiQuat.z);
	}
};
