#pragma once

#include "Window.h"
#include <unordered_map>

class InputManager : public CallbackInterface {

public:
	InputManager(int screenWidth, int screenHeight);
	~InputManager();

	virtual void keyCallback(int key, int scancode, int action, int mods);
	virtual void mouseButtonCallback(int button, int action, int mods);
	virtual void cursorPosCallback(double xpos, double ypos);
	virtual void scrollCallback(double xoffset, double yoffset);
	virtual void windowSizeCallback(int width, int height);

	bool onKeyAction(int key, int action);
	bool onMouseButtonAction(int button, int action);

	void refreshInput();

	glm::vec2 getMousePosition();

private:
	glm::ivec2 m_screenDim;
	glm::vec2 m_screenPos;

	std::unordered_map<int, bool> m_keyPressed;
	std::unordered_map<int, bool> m_keyReleased;

	std::unordered_map<int, bool> m_mouseButtonPressed;
	std::unordered_map<int, bool> m_mouseButtonReleased;


	void setBinds();

};