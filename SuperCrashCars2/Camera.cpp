#include "Camera.h"

Camera::Camera(ShaderProgram& shader, int screenWidth, int screenHeight, glm::vec3 position, glm::vec3 up) :
	m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
	m_aspectRatio((float)screenWidth / (float)screenHeight),
	m_fov(60.0f),
	m_yaw(-90.0f),
	m_pitch(0.0f),
	m_cameraTranslateSens(0.15f),
	m_cameraRotationSens(40.0f),
	m_firstMouse(false),
	m_lastX(0.0f),
	m_lastY(0.0f),
	m_shader(shader)
{
	this->m_position = position;
	this->m_up = up;
	this->UpdateMVP();
}

void Camera::UpdateMVP() {
	this->M = glm::mat4(1.0f);
	this->P = glm::perspective(glm::radians(this->m_fov), this->m_aspectRatio, 0.1f, 1000.0f);
	this->V = glm::lookAt(this->m_position, this->m_position + this->m_front, this->m_up);
}

void Camera::handleTranslation(int key) {
	if (key == GLFW_KEY_W) this->m_position += this->m_cameraTranslateSens * this->m_front;
	if (key == GLFW_KEY_A) this->m_position -= glm::normalize(glm::cross(this->m_front, this->m_up)) * this->m_cameraTranslateSens;
	if (key == GLFW_KEY_S) this->m_position -= this->m_cameraTranslateSens * this->m_front;
	if (key == GLFW_KEY_D) this->m_position += glm::normalize(glm::cross(this->m_front, this->m_up)) * this->m_cameraTranslateSens;
}

void Camera::handleRotation(float xpos, float ypos) {
	if (this->m_firstMouse) {
		this->m_lastX = xpos;
		this->m_lastY = ypos;
		this->m_firstMouse = false;
	}

	float xoffset = xpos - this->m_lastX;
	float yoffset = ypos - this->m_lastY;

	this->m_lastX = xpos;
	this->m_lastY = ypos;

	xoffset *= this->m_cameraRotationSens;
	yoffset *= this->m_cameraRotationSens;

	this->m_yaw += xoffset;
	this->m_pitch += yoffset;

	glm::vec3 front;
	front.x = cos(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
	front.y = sin(glm::radians(this->m_pitch));
	front.z = sin(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
	this->m_front = glm::normalize(front);
}

const glm::vec3& Camera::getPosition() {
	return this->m_position;
}

const float Camera::getYaw() {
	return this->m_yaw;
}

const float Camera::getPitch() {
	return this->m_pitch;
}

void Camera::setPosition(glm::vec3 position) {
	this->m_position = position;
}

void Camera::setYaw(float yaw) {
	this->m_yaw = -90.0f - yaw;
	glm::vec3 front;
	front.x = cos(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
	front.y = sin(glm::radians(this->m_pitch));
	front.z = sin(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
	this->m_front = glm::normalize(front);
}

void Camera::setPitch(float pitch) {
	this->m_pitch = pitch;
	glm::vec3 front;
	front.x = cos(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
	front.y = sin(glm::radians(this->m_pitch));
	front.z = sin(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
	this->m_front = glm::normalize(front);
}

void Camera::render() {
	GLint modelLoc = glGetUniformLocation(this->m_shader, "M");
	GLint viewLoc = glGetUniformLocation(this->m_shader, "V");
	GLint projectionLoc = glGetUniformLocation(this->m_shader, "P");
	this->UpdateMVP();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &this->M[0][0]);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &this->V[0][0]);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &this->P[0][0]);
}

void Camera::resetLastPos() {
	this->m_lastX = 0.0f;
	this->m_lastY = 0.0f;
}
