#include "Time.h"

Time::Time() { 
	totalRenderTime = microseconds(0);
	totalSimTime = microseconds(0);
	averageRenderTime = 0;
	averageSimTime = 0;
	simulations = 0;
	renders = 0;
	
	renderAccum = microseconds(0);
	physicsAccum = microseconds(0);
	
	shouldRender = false;
	shouldSimulate = false;

	// oscillation stuff
	time_point<steady_clock> startOfProgram = steady_clock::now();
}

void Time::update() {
	static time_point<steady_clock> lastTime = steady_clock::now();
	time_point<steady_clock> currentTime = steady_clock::now();
	deltaTime = duration_cast<microseconds>(currentTime - lastTime);
	renderAccum += deltaTime;
	physicsAccum += deltaTime;
	lastTime = currentTime;
	// 16666.. microseconds = 16.666 ms is one frame at 60fps OR 30fps 33.333 ms for 30fps
	if (renderAccum > microseconds(FPSArray[multiplayer])) {
		shouldRender = true;
		renderAccum = renderAccum % microseconds(FPSArray[multiplayer]);
	}
	//simulate physics at 120fps
	if (physicsAccum > microseconds(8333)) {
		shouldSimulate = true;
		physicsAccum = physicsAccum % microseconds(8333);
	}

}

bool Time::fastOscilator() {
	return (duration_cast<milliseconds>(steady_clock::now() - startOfProgram).count() % 100) < 50;
}

bool Time::slowOscilator() {
	return (duration_cast<milliseconds>(steady_clock::now() - startOfProgram).count() % 400) < 200;
}

void Time::endRenderTimer() {
	renders++;
	totalRenderTime += duration_cast<microseconds>(steady_clock::now() - renderDelta);
	//Log::info("Frame Rendered");
	shouldRender = false;
	averageRenderTime = (int)totalRenderTime.count() / renders;
}

void Time::endSimTimer() {
	simulations++;
	totalSimTime += duration_cast<microseconds>(steady_clock::now() - simulateDelta);
	//Log::info("Physics Simulated");
	shouldSimulate = false;
	averageSimTime = (int)totalSimTime.count() / simulations;
}

void Time::startRenderTimer() {
	renderDelta = steady_clock::now();
}

void Time::startSimTimer() {
	simulateDelta = steady_clock::now();
}

void Time::displayDeltaTime() {
	Log::info("Update took {} microseconds", deltaTime.count());
}

void Time::resetStats() {
	totalRenderTime = microseconds(0);
	totalSimTime = microseconds(0);
	averageRenderTime = 0;
	averageSimTime = 0;
	simulations = 0;
	renders = 0;
}

time_point<steady_clock> Time::getTime() {
	return steady_clock::now();
}

void Time::toMultiplayerMode()
{
	multiplayer = 1;
}

void Time::toSinglePlayerMode()
{
	multiplayer = 0;
}


