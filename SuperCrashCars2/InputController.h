#pragma once

#include "window.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "PVehicle.h"
#include <iostream>

class InputController
{
public:
	InputController(int playerID);
	InputController();
	~InputController();
	void testInput(const float* axes, const unsigned char* buttons);
	void XboxInput(PVehicle& p1, int throttle);
	void PS4Input(PVehicle& p1, int throttle);
	void NSInput(PVehicle& p1, int throttle);

private:
	int id;
	int axesCount;
	const float* axes;
	int buttonCount;
	const unsigned char* buttons;
	const char* name;

	void readInput();
};

