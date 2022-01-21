#pragma once

#include <memory>
#include "ShaderProgram.h"


class Utils {

public:
	static Utils& instance() { static Utils shared; return shared; }

	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;
	
	std::shared_ptr<ShaderProgram> shader = nullptr;

private:
	Utils() {};
	Utils(Utils const& other) = delete;
	Utils(Utils&& other) = delete;

};