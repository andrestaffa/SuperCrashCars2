#pragma once

#include "window.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "PVehicle.h"
#include <iostream>

enum Controller {
	PS4,
	XBOX,
	NS
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
	void uniController(bool isInGame, PVehicle& player);
	bool selHeld, startHeld, xHeld, upHeld, downHeld, rightHeld, leftHeld = 0;
	bool connected; // for audio and menu
private:



	int id;
	int axesCount;
	const float* axis;
	int buttonCount;
	bool ready;
	const unsigned char* buttons;
	const char* name;

	void readInput();

};

