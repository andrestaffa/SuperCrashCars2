#pragma once

#include <Windows.h>
#include <chrono>
#include "Log.h"

using namespace std::chrono;

class Time {

public:
	inline static microseconds totalRenderTime = microseconds(0);
	inline static microseconds totalSimTime = microseconds(0);
	inline static int averageRenderTime = 0;
	inline static int averageSimTime = 0;
	inline static int simulations = 0;
	inline static int renders = 0;

	inline static microseconds deltaTime;
	inline static microseconds renderAccum = microseconds(0);
	inline static microseconds physicsAccum = microseconds(0);
	inline static time_point<steady_clock> renderDelta;
	inline static time_point<steady_clock> simulateDelta;
	inline static bool shouldRender = false;
	inline static bool shouldSimulate = false;


	static void update() {
		static time_point<steady_clock> lastTime = steady_clock::now();
		time_point<steady_clock> currentTime = steady_clock::now();
		Time::deltaTime = duration_cast<microseconds>(currentTime - lastTime);
		Time::renderAccum += deltaTime;
		Time::physicsAccum += deltaTime;
		lastTime = currentTime;
		// 16666.. microseconds = 16.666 ms is one frame at 60fps
		if (renderAccum > microseconds(16666)) {
			Time::shouldRender = true;
			Time::renderAccum = Time::renderAccum % microseconds(16666);
		}
		//simulate physics at 120fps
		if (physicsAccum > microseconds(8333)) {
			Time::shouldSimulate = true;
			Time::physicsAccum = Time::physicsAccum % microseconds(8333);
		}
	}

	static void endRenderTimer() {
		Time::renders++;
		Time::totalRenderTime += duration_cast<microseconds>(steady_clock::now() - Time::renderDelta);
		//Log::info("Frame Rendered");
		Time::shouldRender = false;
		Time::averageRenderTime = (int)totalRenderTime.count() / Time::renders;
	}

	static void endSimTimer() {
		Time::simulations++;
		Time::totalSimTime += duration_cast<microseconds>(steady_clock::now() - Time::simulateDelta);
		//Log::info("Physics Simulated");
		Time::shouldSimulate = false;
		Time::averageSimTime = (int)totalSimTime.count() / Time::simulations;
	}

	static void startRenderTimer() {
		Time::renderDelta = steady_clock::now();
	}

	static void startSimTimer() {
		Time::simulateDelta = steady_clock::now();
	}

	static void displayDeltaTime() {
		Log::info("Update took {} microseconds", Time::deltaTime.count());
	}

	static void resetStats() {
		Time::totalRenderTime = microseconds(0);
		Time::totalSimTime = microseconds(0);
		Time::averageRenderTime = 0;
		Time::averageSimTime = 0;
		Time::simulations = 0;
		Time::renders = 0;
	} 

	static time_point<steady_clock> getTime() {
		return steady_clock::now();
	}

};