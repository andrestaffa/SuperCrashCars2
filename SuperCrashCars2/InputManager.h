#pragma once

#include "Window.h"

class InputManager : public CallbackInterface {

public:
	InputManager(int screenWidth, int screenHeight);
	~InputManager();

	glm::vec2 getMousePosition();

	bool onKeyDown(int key);
	bool onMouseButtonDown(int button);
	void refreshInput();

	virtual void keyCallback(int key, int scancode, int action, int mods);
	virtual void cursorPosCallback(double xpos, double ypos);
	virtual void scrollCallback(double xoffset, double yoffset);
	virtual void windowSizeCallback(int width, int height);

private:
	glm::ivec2 m_screenDim;
	glm::vec2 m_screenPos;

};