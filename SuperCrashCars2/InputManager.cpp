#include "InputManager.h"


InputManager::InputManager(int screenWidth, int screenHeight) :
	m_screenDim(screenWidth, screenHeight),
	m_screenPos(0.0f, 0.0f)
{
	this->setBinds();
}

void InputManager::keyCallback(int key, int scancode, int action, int mods) {
	this->m_keyPressed[key] = (action == GLFW_PRESS || action == GLFW_REPEAT);
	this->m_keyReleased[key] = (action == GLFW_RELEASE);
}

void InputManager::mouseButtonCallback(int button, int action, int mods) {
	this->m_mouseButtonPressed[button] = (action == GLFW_PRESS || action == GLFW_REPEAT);
	this->m_mouseButtonReleased[button] = (action == GLFW_RELEASE);
}

void InputManager::cursorPosCallback(double xpos, double ypos) {
	this->m_screenPos.x = (float)xpos;
	this->m_screenPos.y = (float)ypos;
}

void InputManager::scrollCallback(double xoffset, double yoffset) {}

void InputManager::windowSizeCallback(int width, int height) {
	CallbackInterface::windowSizeCallback(width, height);
	this->m_screenDim = glm::ivec2(width, height);
	//glViewport(0, 0, width, height);
}

bool InputManager::onKeyAction(int key, int action) {
	return (action == GLFW_PRESS || action == GLFW_REPEAT) ? this->m_keyPressed[key] : this->m_keyReleased[key];
}

bool InputManager::onMouseButtonAction(int button, int action) {
	return (action == GLFW_PRESS || action == GLFW_REPEAT) ? this->m_mouseButtonPressed[button] : this->m_mouseButtonReleased[button];
}

void InputManager::refreshInput() {
	for (auto& it : this->m_mouseButtonPressed) it.second = false;
	for (auto& it : this->m_mouseButtonReleased) it.second = false;

	for (auto& it : this->m_keyPressed) it.second = false;
	for (auto& it : this->m_keyReleased) it.second = false;
}

glm::vec2 InputManager::getMousePosition() {
	glm::vec2 startingVec = this->m_screenPos;
	glm::vec2 shiftedVec = startingVec + glm::vec2(0.5f, 0.5f);
	glm::vec2 scaledVec = shiftedVec / glm::vec2(this->m_screenDim);
	glm::vec2 reversedY = glm::vec2(scaledVec.x, 1.0f - scaledVec.y);
	glm::vec2 finalVec = reversedY * 2.0f - glm::vec2(1.0f, 1.0f);
	return finalVec;
}

void InputManager::setBinds() {
	this->m_mouseButtonPressed = this->m_mouseButtonReleased = std::unordered_map<int, bool>({
				{GLFW_MOUSE_BUTTON_LEFT, false},
				{GLFW_MOUSE_BUTTON_RIGHT, false}
		});
	this->m_keyPressed = this->m_keyReleased = std::unordered_map<int, bool>({
				{GLFW_KEY_W, false},
				{GLFW_KEY_A, false},
				{GLFW_KEY_S, false},
				{GLFW_KEY_D, false},
				{GLFW_KEY_Q, false},
				{GLFW_KEY_E, false},
				{GLFW_KEY_C, false},
				{GLFW_KEY_SPACE, false},
				{GLFW_KEY_UP, false},
				{GLFW_KEY_DOWN, false},
				{GLFW_KEY_LEFT, false},
				{GLFW_KEY_RIGHT, false},
				{GLFW_KEY_LEFT_SHIFT, false},
				{GLFW_KEY_LEFT_CONTROL, false}
		});
}


InputManager::~InputManager() {}
