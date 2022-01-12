#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/gtc/type_ptr.hpp"

#include "ShaderProgram.h"

class Camera {

public:
	Camera(ShaderProgram& shader, int screenWidth, int screenHeight, glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

	void handleTranslation(int key);
	void handleRotation(float xpos, float ypos);

	void render();
	void resetLastPos();

private:
	glm::mat4 P, V, M;
	float m_cameraTranslateSens, m_cameraRotationSens;

	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;

	ShaderProgram& m_shader;

	float m_aspectRatio;
	float m_fov;

	float m_yaw;
	float m_pitch;

	void UpdateMVP();

	bool m_firstMouse;
	float m_lastX;
	float m_lastY;


};