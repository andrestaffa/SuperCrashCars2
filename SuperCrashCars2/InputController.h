#pragma once

#include "window.h"

class InputController
{
public:
	InputController(int playerID);

	~InputController();


private:
	int countA;
	int axesCount;
	const float* axes;// = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
	int buttonCount;
	const unsigned char* buttons; 
};

