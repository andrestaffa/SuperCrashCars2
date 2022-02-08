#include "InputController.h"

InputController::InputController()
{
	//Defalut constructor
}

InputController::InputController(int playerID) 
{
	this->id = playerID;
	this->name = glfwGetJoystickName(playerID);
	this->axesCount = 0;
	this->axes = glfwGetJoystickAxes(playerID, &this->axesCount);
	this->buttonCount = 0;
	this->buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
}

InputController::~InputController() {}

/* This is the test code for input controller mapping
 *
 * Xbox:
 * Button[0]--A, Button[1]--B, Button[2]--X, Button[3]--Y, Button[4]--LB, Button[5]--RB,
 * Button[6]--BACK(Left-Middle), Button[7]--START(Right-Middle), Button[8]--LSB, Button[9]--RSB,
 * Button[10]--UP, Button[11]--RIGHT, Button[12]--Down, Button[13]--LEFT.
 * axes[0]--Left Stick X Axis, axes[1]--Left Stick Y Axis(up is -1 for myself), axes[2]--Right Stick X Axis,
 * axes[3]--Right Trigger, axes[4]--Left Trigger, axes[5]--Right Stick Y Axis(up is -1 for myself).
 *
 *
 * PS4:
 * Button[0]--Square, Button[1]--X, Button[2]--O, Button[3]--Triangle, Button[4]--L1, Button[5]--R1,
 * Button[6]--L2, Button[7]--R2, Button[8]--SHARE, Button[9]--OPT,
 * Button[10]--LSB, Button[11]--RSB, Button[12]--PlayStation, Button[13]--Touchpad,
 * Button[14]--UP, Button[15]--RIGHT, Button[16]--Down, Button[17]--LEFT.
 * axes[0]--Left Stick X Axis, axes[1]--Left Stick Y Axis, axes[2]--Right Stick X Axis,
 * axes[3]--Left Trigger, axes[4]--Right Trigger, axes[5]--Right Stick Y Axis(up is -1 for myself).
 *
 *
 * NS:
 * Button[0]--B, Button[1]--A, Button[2]--Y, Button[3]--X, Button[4]--L, Button[5]--R,
 * Button[6]--ZL, Button[7]--ZR, Button[8]--'-', Button[9]--'+', Button[10]--LSB, Button[11]--RSB,
 * Button[12]--Home, Button[13]--ScreenShot,Button[14]--???, Button[15]--???, Button[16]--Up,
 * Button[17]--Righ, Button[18]--Down, Button[19]--Left.
 * axes[0]--Left Stick X Axis, axes[1]--Left Stick Y Axis,
 * axes[2]--Right Stick X Axis, axes[3]--Left Trigger.
 */
void InputController::testInput(const float* axes, const unsigned char* buttons) {
	if (GLFW_PRESS == buttons[0])
	{
		std::cout << "0" << std::endl;
	}
	else if (GLFW_PRESS == buttons[1])
	{
		std::cout << "1" << std::endl;
	}
	else if (GLFW_PRESS == buttons[2])
	{
		std::cout << "2" << std::endl;
	}
	else if (GLFW_PRESS == buttons[3])
	{
		std::cout << "3" << std::endl;
	}
	else if (GLFW_PRESS == buttons[4])
	{
		std::cout << "4" << std::endl;
	}
	else if (GLFW_PRESS == buttons[5])
	{
		std::cout << "5" << std::endl;
	}
	else if (GLFW_PRESS == buttons[6])
	{
		std::cout << "6" << std::endl;
	}
	else if (GLFW_PRESS == buttons[7])
	{
		std::cout << "7" << std::endl;
	}
	else if (GLFW_PRESS == buttons[8])
	{
		std::cout << "8" << std::endl;
	}
	else if (GLFW_PRESS == buttons[9])
	{
		std::cout << "9" << std::endl;
	}
	else if (GLFW_PRESS == buttons[10])
	{
		std::cout << "10" << std::endl;
	}
	else if (GLFW_PRESS == buttons[11])
	{
		std::cout << "11" << std::endl;
	}
	else if (GLFW_PRESS == buttons[12])
	{
		std::cout << "12" << std::endl;
	}
	else if (GLFW_PRESS == buttons[13])
	{
		std::cout << "13" << std::endl;
	}
	else if (GLFW_PRESS == buttons[14])
	{
		std::cout << "14" << std::endl;
	}
	else if (GLFW_PRESS == buttons[15])
	{
		std::cout << "15" << std::endl;
	}
	else if (GLFW_PRESS == buttons[16])
	{
		std::cout << "16" << std::endl;
	}
	else if (GLFW_PRESS == buttons[17])
	{
		std::cout << "17" << std::endl;
	}
	else if (GLFW_PRESS == buttons[18])
	{
		std::cout << "18" << std::endl;
	}
	else if (GLFW_PRESS == buttons[19])
	{
		std::cout << "19" << std::endl;
	}

	//std::cout << "Left Stick X Axis: " << axes[0] << std::endl;
	//std::cout << "Left Stick Y Axis: " << axes[1] << std::endl;
	//std::cout << "Right Stick X Axis: " << axes[2] << std::endl;
	//std::cout << "Right Stick Y Axis: " << axes[5] << std::endl;
	//std::cout << "Right Trigger/R2: " << axes[4] << std::endl;
	//std::cout << "Left Trigger/L2: " << axes[3] << std::endl;
}

void InputController::XboxInput(PVehicle& p1, int throttle) {
	//Xbox con code
	if (abs(axes[0]) > 0.15) {
		if (axes[0] < 0) {
			p1.turnLeft(throttle * abs(axes[0]) * 0.5);
		}
		else
		{
			p1.turnRight(throttle * abs(axes[0]) * 0.5);
		}
	}
	if (GLFW_PRESS == buttons[0])
	{
		p1.handbrake();
	}
	if (axes[4] != -1)
	{
		p1.reverse(throttle * 0.5f);
	}
	if (axes[5] != -1)
	{
		p1.accelerate(throttle);
	}
	//if (abs(p1.getPosition().z) >= 101.0f || abs(p1.getPosition().x) >= 101.0) p1.removePhysics();

	return;
}

void InputController::PS4Input(PVehicle& p1, int throttle) {

	if (abs(axes[0]) > 0.15) {
		if (axes[0] < 0) {
			p1.turnLeft(throttle * abs(axes[0]) * 0.5);
		}
		else
		{
			p1.turnRight(throttle * abs(axes[0]) * 0.5);
		}
	}
	if (GLFW_PRESS == buttons[0])
	{
		p1.handbrake();
	}
	if (axes[3] != -1)
	{
		p1.reverse(throttle * 0.5f);
	}
	if (axes[4] != -1)
	{
		p1.accelerate(throttle);
	}
	//if (abs(p1.getPosition().z) >= 101.0f || abs(p1.getPosition().x) >= 101.0) p1.removePhysics();
}

void InputController::NSInput(PVehicle& p1, int throttle) {

	if (abs(axes[0]) > 0.15) {
		if (axes[0] < 0) {
			p1.turnLeft(throttle * abs(axes[0]) * 0.5);
		}
		else
		{
			p1.turnRight(throttle * abs(axes[0]) * 0.5);
		}
	}
	if (GLFW_PRESS == buttons[0])
	{
		p1.handbrake();
	}
	if (GLFW_PRESS == buttons[6])
	{
		p1.reverse(throttle * 0.5f);
	}
	if (GLFW_PRESS == buttons[7])
	{
		p1.accelerate(throttle);
	}
	//if (abs(p1.getPosition().z) >= 101.0f || abs(p1.getPosition().x) >= 101.0) p1.removePhysics();
}
