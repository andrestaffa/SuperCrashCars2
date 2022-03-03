#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "Utils.h"
#include "Log.h"
#include <cmath>
#define M_PI 3.14159265358979323846

class Camera {

public:
	Camera(int screenWidth, int screenHeight, const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f), const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

	void handleTranslation(int key);
	void handleRotation(float xpos, float ypos);

	const glm::vec3& getPosition() const;
	const float getYaw() const;
	const float getPitch() const;
	const glm::mat4 getViewMat();
	const glm::mat4 getPerspMat();

	void setPosition(const glm::vec3& position);
	void setYaw(float yaw);
	void setPitch(float pitch);

	void UpdateVP();
	void resetLastPos();

	void updateCamera(glm::vec3 newPosition, glm::vec3 frontVector);

	void sendMatricesToShader();



private:
	glm::mat4 P, V;
	float m_cameraTranslateSens, m_cameraRotationSens, m_pos_coeff, m_rot_coeff, m_camTheta, m_camThetaGoal;

	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;

	float m_aspectRatio;
	float m_fov;

	float m_yaw;
	float m_pitch;

	bool m_firstMouse;
	float m_lastX;
	float m_lastY;

	glm::vec3 m_front_goal;
	glm::vec3 m_position_goal;

	void updateTheta();

};