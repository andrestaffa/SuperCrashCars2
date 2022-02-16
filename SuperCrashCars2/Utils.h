#pragma once

#include "PxPhysicsAPI.h"
#include <memory>
#include "ShaderProgram.h"

class Utils {

public:
	static Utils& instance() { static Utils shared; return shared; }

	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;
	
	std::shared_ptr<ShaderProgram> shader = nullptr;

	physx::PxVec3 glmVec3ToPhysxVec3(glm::vec3 vec) {
		return physx::PxVec3(vec.x, vec.y, vec.z);
	}

	glm::vec3 physxVec3ToGlmVec3(physx::PxVec3 vec) {
		return glm::vec3(vec.x, vec.y, vec.z);
	}


private:
	Utils() {};
	Utils(Utils const& other) = delete;
	Utils(Utils&& other) = delete;

};