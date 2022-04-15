#include "Camera.h"

Camera::Camera(int screenWidth, int screenHeight, const glm::vec3& position, const glm::vec3& up) :
	m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
	m_front_goal(glm::vec3(0.0f, 0.0f, -1.0f)),
	m_aspectRatio((float)screenWidth / (float)screenHeight),
	m_fov(80.0f),
	m_yaw(-90.0f),
	m_pitch(0.0f),
	m_cameraTranslateSens(0.15f),
	m_cameraRotationSens(40.0f),
	m_firstMouse(false),
	m_lastX(0.0f),
	m_lastY(0.0f),
	m_pos_coeff(0.90),
	m_rot_coeff(10.0f), // the larger, the slower camera rotates.
	m_camTheta(float(3.0 * M_PI / 2.0))
{
	this->m_position = position;
	this->m_position_goal = position;
	this->m_up = up;
	this->UpdateVP();
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
const glm::mat4 Camera::getViewMat() {
	return V;
}
const glm::mat4 Camera::getPerspMat() {
	return P;
}
const glm::vec3 Camera::getFront() {
	return this->m_front;
}

void Camera::setPosition(const glm::vec3& position) {
	this->m_position = position;
}
void Camera::setYaw(float yaw) {
	this->m_yaw = yaw;
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

// current theta
// camera position is playerPos + rotate(frontVector, theta)
// only update the theta, don't mess with the position
// interpolate the theta via a function, don't interpolate the position itself.

void Camera::updateTheta() {
	float delta;
	// make sure camera theta is always between 0 and 2pi
	m_camTheta = (fmodf(m_camTheta, 2.f * (M_PI)));
	if (m_camTheta < 0) {
		m_camTheta += 2.f *(M_PI);
	}

	//set diff
	float diff = abs(m_camTheta - m_camThetaGoal);

	// set delta
	if (diff > (M_PI)) {
		delta = (2.f * (M_PI) - diff ) / m_rot_coeff;
	}
	else {
		delta = diff / m_rot_coeff;
	}

	if (diff < M_PI && m_camThetaGoal > m_camTheta) {
		m_camTheta = m_camTheta + delta;
		// Log::debug("<, >");
	}
	else if (diff < M_PI && m_camThetaGoal <= m_camTheta) {
		m_camTheta = m_camTheta - delta;
		// Log::debug("<, >=");
	}
	else if (diff >= M_PI && m_camThetaGoal < m_camTheta) {
		m_camTheta = m_camTheta + delta;
		// Log::debug(">=, <");
	}
	else if (diff >= M_PI && m_camThetaGoal >= m_camTheta) {
		m_camTheta = m_camTheta - delta;
		// Log::debug(">=, <=");
	}
}

void Camera::updateCameraPosition(glm::vec3 carPosition, glm::vec3 frontVector) {
	// calculate the new angle
	m_camThetaGoal = (float)(M_PI)+glm::orientedAngle(glm::normalize(glm::vec2(frontVector.x, frontVector.z)), glm::vec2(1.0f, 0.0f)); 
	// recalculate the rotation coeff every frame based on the Y value of front vec
	// we want the camera to rotate slower when the player is flipping int he Y direction
	m_rot_coeff = 15.f + (35.f * abs(frontVector.y));
	// do the magic 
	this->updateTheta();

	// calculate the camera vector from angle. this is like frontVec, but without the y component.
	// we calculate it from angle instead of just using frontVector to dampen rotations around a circle as opposed to cutting through it.
	glm::vec2 cameraVec2 = glm::rotate(glm::vec2(1.0f, 0.0f), m_camTheta);
	glm::vec3 cameraVec = glm::vec3(-cameraVec2.x, 0.0f, cameraVec2.y);
	// set new posiiton and angle
	this->setPosition(carPosition - (cameraVec * 15.f) + glm::vec3(0.0f, 7.f, 0.0f));
	this->m_front = glm::vec3(cameraVec.x, -0.4f + frontVector.y * 0.2f, cameraVec.z);

	this->UpdateVP(); // update the view and persp of camera

}

void Camera::sendMatricesToShader() {
	GLint viewLoc = glGetUniformLocation(*Utils::instance().shader, "V");
	GLint projectionLoc = glGetUniformLocation(*Utils::instance().shader, "P");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &this->V[0][0]);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &this->P[0][0]);
}

void Camera::UpdateVP() {
	this->P = glm::perspective(glm::radians(this->m_fov), this->m_aspectRatio, 0.1f, 2000.0f);
	this->V = glm::lookAt(this->m_position, this->m_position + this->m_front, this->m_up);
}

void Camera::resetLastPos() {
	this->m_lastX = 0.0f;
	this->m_lastY = 0.0f;
}

