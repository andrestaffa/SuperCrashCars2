#pragma once

#include <random>

#include "PxPhysicsAPI.h"
#include <memory>
#include "ShaderProgram.h"

class Utils {

public:
	static Utils& instance() { static Utils shared; return shared; }

	const int SCREEN_WIDTH = 1600; // change to 1920
	const int SCREEN_HEIGHT = 900; // change to 1000
	
	std::shared_ptr<ShaderProgram> shader = nullptr;

	physx::PxVec3 glmToPxVec3(glm::vec3 vec) {
		return physx::PxVec3(vec.x, vec.y, vec.z);
	}

	glm::vec3 pxToGlmVec3(physx::PxVec3 vec) {
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	template<typename T>
	T random(T range_from, T range_to) {
		std::random_device                  rand_dev;
		std::mt19937                        generator(rand_dev());
		std::uniform_int_distribution<T>    distr(range_from, range_to);
		return distr(generator);
	}


private:
	Utils() {};
	Utils(Utils const& other) = delete;
	Utils(Utils&& other) = delete;

};