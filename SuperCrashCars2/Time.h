#pragma once

#include <Windows.h>
#include "Log.h"

class Time {

public:

	inline static float fps = 0.0f;
	inline static float deltaTime = 0.0f;

	static void update() {
		static float framesPerSecond = 0.0f;
		static float lastTime = 0.0f;
		float currentTime = GetTickCount64() * 0.001f;
		++framesPerSecond;
		if (currentTime - lastTime > 1.0f) {
			lastTime = currentTime;
			Time::fps = framesPerSecond;
			Time::deltaTime = 1 / Time::fps;
			framesPerSecond = 0;
		}
	}

	static bool interval(float seconds) {
		static float lastTime = 0.0f;
		float currentTime = GetTickCount64() * 0.001f;
		if (currentTime - lastTime > seconds) {
			lastTime = currentTime;
			return true;
		}
		return false;
	}

	static void displayFPS() {
		Log::info("{} FPS", Time::fps);
	}

	static void displayDeltaTime() {
		Log::info("{} ms", Time::deltaTime);
	}

};