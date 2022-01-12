#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Utils.h"
#include "InputManager.h"
#include "Camera.h"
#include "GLMesh.h"

#include "Vehicle.h"
#include "PhysicsRenderer.h"

int main(int argc, char** argv) {
	Log::info("Starting Game...");

	glfwInit();
	
	Window window(Utils::shared().SCREEN_WIDTH, Utils::shared().SCREEN_HEIGHT, "Super Crash Cars 2");
	GLDebug::enable();

	std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>(Utils::shared().SCREEN_WIDTH, Utils::shared().SCREEN_HEIGHT);
	window.setCallbacks(inputManager);

	ShaderProgram shader("shaders/shader_vertex.vert", "shaders/shader_fragment.frag");

	Camera camera(shader, Utils::shared().SCREEN_WIDTH, Utils::shared().SCREEN_HEIGHT, glm::vec3(-2.0f, 4.0f, 10.0f));

	initPhysics();
	physx::PxVehicleDrive4W* vehicle = instantiateVehicle();
	PhysicsRenderer physicsRenderer;
	float throttle = 1.0f;

	GLMesh plane(shader), sphere(shader);
	plane.createPlane(100, glm::vec3(0.0f));
	sphere.createSphere(0.5f, 10, glm::vec3(0.0f, 1.0f, 0.0f));

	while (!window.shouldClose()) {

		stepPhysics(1.0f / 100.0f);
		releaseAllControls();

		glfwPollEvents();
		
		if (inputManager->onMouseButtonAction(GLFW_MOUSE_BUTTON_RIGHT, GLFW_REPEAT))
			camera.handleRotation(inputManager->getMousePosition().x, inputManager->getMousePosition().y);
		else if (inputManager->onMouseButtonAction(GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE)) 
			camera.resetLastPos();
	
		if (inputManager->onKeyAction(GLFW_KEY_W, GLFW_PRESS)) camera.handleTranslation(GLFW_KEY_W);
		if (inputManager->onKeyAction(GLFW_KEY_A, GLFW_PRESS)) camera.handleTranslation(GLFW_KEY_A);
		if (inputManager->onKeyAction(GLFW_KEY_S, GLFW_PRESS)) camera.handleTranslation(GLFW_KEY_S);
		if (inputManager->onKeyAction(GLFW_KEY_D, GLFW_PRESS)) camera.handleTranslation(GLFW_KEY_D);

		if (inputManager->onKeyAction(GLFW_KEY_UP, GLFW_PRESS)) accelerate(throttle);
		if (inputManager->onKeyAction(GLFW_KEY_DOWN, GLFW_PRESS)) brake(throttle);
		if (inputManager->onKeyAction(GLFW_KEY_LEFT, GLFW_PRESS)) turnLeft(throttle * 0.5f);
		if (inputManager->onKeyAction(GLFW_KEY_RIGHT, GLFW_PRESS)) turnRight(throttle * 0.5f);
		if (inputManager->onKeyAction(GLFW_KEY_SPACE, GLFW_PRESS)) handbrake(1.0f);


		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		shader.use();

		camera.render();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		plane.render();

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		physicsRenderer.renderActor(*vehicle->getRigidDynamicActor(), sphere);

		glDisable(GL_FRAMEBUFFER_SRGB);
		window.swapBuffers();

	}

	cleanupPhysics();
	glfwTerminate();
	return 0;
}