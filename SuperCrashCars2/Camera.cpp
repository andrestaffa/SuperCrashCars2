#include "Camera.h"

Camera::Camera(int screenWidth, int screenHeight, const glm::vec3& position, const glm::vec3& up) :
	m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
	m_front_goal(glm::vec3(0.0f, 0.0f, -1.0f)),
	m_aspectRatio((float)screenWidth / (float)screenHeight),
	m_fov(60.0f),
	m_yaw(-90.0f),
	m_pitch(0.0f),
	m_cameraTranslateSens(0.15f),
	m_cameraRotationSens(40.0f),
	m_firstMouse(false),
	m_lastX(0.0f),
	m_lastY(0.0f),
	cam_coeff(0.07)
{
	this->m_position = position;
	this->m_position_goal = position;
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
	if (key == GLFW_KEY_LEFT_SHIFT) this->m_position += this->m_cameraTranslateSens * this->m_up;
	if (key == GLFW_KEY_LEFT_CONTROL) this->m_position -= this->m_cameraTranslateSens * this->m_up;
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

const glm::vec3& Camera::getPosition() const {
	return this->m_position;
}

const float Camera::getYaw() const {
	return this->m_yaw;
}

const float Camera::getPitch() const {
	return this->m_pitch;
}

void Camera::setPosition(const glm::vec3& position) {
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
	GLint modelLoc = glGetUniformLocation(*Utils::instance().shader, "M");
	GLint viewLoc = glGetUniformLocation(*Utils::instance().shader, "V");
	GLint projectionLoc = glGetUniformLocation(*Utils::instance().shader, "P");
	this->UpdateMVP();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &this->M[0][0]);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &this->V[0][0]);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &this->P[0][0]);
}

void Camera::resetLastPos() {
	this->m_lastX = 0.0f;
	this->m_lastY = 0.0f;
}

void Camera::updateCamera(glm::vec3 newPosition, glm::vec3 frontVector){
	this->m_position_goal = newPosition - (frontVector * 15.f) + glm::vec3(0.0f, 7.f, 0.0f);
	this->setPosition(m_position * (1.f - cam_coeff) + m_position_goal * cam_coeff);
	 
	this->m_front_goal =  glm::vec3(frontVector.x , -0.4f + frontVector.y * 0.3f, frontVector.z);
	this->m_front = (m_front * (1.f - cam_coeff*1.5f) + m_front_goal * cam_coeff*1.5f);

}