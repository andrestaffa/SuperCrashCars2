#pragma once

class Utils {

public:
	Utils() {}
	static Utils& shared() {
		static Utils instance;
		return instance;
	}

	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;
};