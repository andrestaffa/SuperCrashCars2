#pragma once

#include "window.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "PVehicle.h"
#include <iostream>

// TO DO :
// Make a unified function for both controllers, pass which controller is used instaed of separate functions for each controller
// automatic controller type detection

enum Controller {
	PS4,
	XBOX
};

class InputController
{
public:
	InputController(int playerID);
	InputController();
	~InputController();

	const char* getName();
	int getButtonCount();
	int getAxesCount();
	void testInput();

	void XboxInputInGame(PVehicle& p1);
	void PS4InputInGame(PVehicle& p1);
	void XboxInputInMenu();
	void PS4InputInMenu();
	void NSInputInGame(PVehicle& p1);
	void NSInputInMenu();

private:

	bool selHeld, startHeld, xHeld, upHeld, downHeld = 0;

	int id;
	int axesCount;
	const float* axis;
	int buttonCount;
	const unsigned char* buttons;
	const char* name;

	void readInput();

};

