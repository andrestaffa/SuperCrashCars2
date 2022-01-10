#include "InputManager.h"
#include "Log.h"


InputManager::InputManager(int screenWidth, int screenHeight) :
	m_screenDim(screenWidth, screenHeight),
	m_screenPos(0.0f, 0.0f)
{}

void InputManager::keyCallback(int key, int scancode, int action, int mods) {

}

void  InputManager::cursorPosCallback(double xpos, double ypos) {
	this->m_screenPos.x = (float)xpos;
	this->m_screenPos.y = (float)ypos;
}

void InputManager::scrollCallback(double xoffset, double yoffset) {}

void InputManager::windowSizeCallback(int width, int height) {
	CallbackInterface::windowSizeCallback(width, height);
	this->m_screenDim = glm::ivec2(width, height);
	glViewport(0, 0, width, height);
}

bool InputManager::onKeyDown(int key) {
	return false;
}

bool InputManager::onMouseButtonDown(int button) {
	return false;
}

void InputManager::refreshInput() {

}

glm::vec2 InputManager::getMousePosition() {
	glm::vec2 startingVec = this->m_screenPos;
	glm::vec2 shiftedVec = startingVec + glm::vec2(0.5f, 0.5f);
	glm::vec2 scaledVec = shiftedVec / glm::vec2(this->m_screenDim);
	glm::vec2 reversedY = glm::vec2(scaledVec.x, 1.0f - scaledVec.y);
	glm::vec2 finalVec = reversedY * 2.0f - glm::vec2(1.0f, 1.0f);
	return finalVec;
}


InputManager::~InputManager() {}
