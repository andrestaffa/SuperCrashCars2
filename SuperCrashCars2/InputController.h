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

	void testInput();

	void XboxInput(PVehicle& p1);
	void PS4Input(PVehicle& p1);
	void NSInput(PVehicle& p1);

private:
	int id;
	int axesCount;
	const float* axis;
	int buttonCount;
	const unsigned char* buttons;
	const char* name;

	void readInput();
};

