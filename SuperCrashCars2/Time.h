#pragma once

#include <Windows.h>
#include <chrono>
#include "Log.h"

using namespace std::chrono;

class Time{
public:
	Time();
	~Time() {};

	microseconds totalRenderTime = microseconds(0);
	microseconds totalSimTime = microseconds(0);
	int averageRenderTime = 0;
	int averageSimTime = 0;
	int simulations = 0;
	int renders = 0;

	microseconds deltaTime;
	microseconds renderAccum = microseconds(0);
	microseconds physicsAccum = microseconds(0);
	time_point<steady_clock> renderDelta;
	time_point<steady_clock> simulateDelta;
	bool shouldRender = false;
	bool shouldSimulate = false;

	// oscillation stuff
	time_point<steady_clock> startOfProgram;

	void update();
	void endRenderTimer();
	void endSimTimer();
	void startRenderTimer();
	void startSimTimer();
	void displayDeltaTime();
	void resetStats();
	time_point<steady_clock> getTime();
	void initOscilators();

private:
	
};