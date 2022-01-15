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

#include "PVehicle.h"
#include "PDynamic.h"
#include "PStatic.h"

/* TODO:
		- Implement player camera.
		- Start implementation of Open Asset Import Library.
		- Turn off collision when off plane. (gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, pm.gAllocator))
*/

int main(int argc, char** argv) {
	Log::info("Starting Game...");

	// OpenGL
	glfwInit();
	Window window(Utils::shared().SCREEN_WIDTH, Utils::shared().SCREEN_HEIGHT, "Super Crash Cars 2");
	GLDebug::enable();
	std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>(Utils::shared().SCREEN_WIDTH, Utils::shared().SCREEN_HEIGHT);
	window.setCallbacks(inputManager);
	ShaderProgram shader = ShaderProgram("shaders/shader_vertex.vert", "shaders/shader_fragment.frag");

	Camera editorCamera = Camera(shader, Utils::shared().SCREEN_WIDTH, Utils::shared().SCREEN_HEIGHT, glm::vec3(-2.0f, 4.0f, 10.0f));

	GLMesh plane(shader, GL_FILL), tires(shader), body(shader), obstacleMesh(shader), ball(shader);
	plane.createPlane(100, glm::vec3(0.0f));
	tires.createSphere(0.5f, 10, glm::vec3(0.0f, 1.0f, 0.0f));
	body.createCube(0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
	body.scale(glm::vec3(2.0f, 2.0f, 3.0f));
	obstacleMesh.createCube(1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ball.createSphere(1.0f, 10, glm::vec3(1.0f, 0.0f, 0.0f));

	// Physx
	float throttle = 1.0f;
	PhysicsManager pm = PhysicsManager(1.0/60.0f);
	PVehicle player = PVehicle(pm, PxVec3(0.0f, 0.0f, 0.0f));
	PDyanmic obstacle_d = PDyanmic(pm, PxSphereGeometry(1), PxVec3(-20.0f, 20.0f, -10.0f));
	PStatic obstacle_s = PStatic(pm, PxBoxGeometry(1.0f, 1.0f, 1.0f), PxVec3(-20.0f, 0.0f, -20.0f));

	Camera playerCamera = Camera(shader, Utils::shared().SCREEN_WIDTH, Utils::shared().SCREEN_HEIGHT, glm::vec3(player.getPosition().x, player.getPosition().y + 5.0f, player.getPosition().z + 10.0));
	playerCamera.setPitch(-30.0f);

	while (!window.shouldClose()) {

		pm.simulate();
		player.update();

		glfwPollEvents();
		
		if (inputManager->onMouseButtonAction(GLFW_MOUSE_BUTTON_RIGHT, GLFW_REPEAT))
			editorCamera.handleRotation(inputManager->getMousePosition().x, inputManager->getMousePosition().y);
		else if (inputManager->onMouseButtonAction(GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE)) 
			editorCamera.resetLastPos();
	
		if (inputManager->onKeyAction(GLFW_KEY_W, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_W);
		if (inputManager->onKeyAction(GLFW_KEY_A, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_A);
		if (inputManager->onKeyAction(GLFW_KEY_S, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_S);
		if (inputManager->onKeyAction(GLFW_KEY_D, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_D);

		if (inputManager->onKeyAction(GLFW_KEY_UP, GLFW_PRESS)) player.accelerate(throttle);
		if (inputManager->onKeyAction(GLFW_KEY_DOWN, GLFW_PRESS)) player.brake(throttle);
		if (inputManager->onKeyAction(GLFW_KEY_LEFT, GLFW_PRESS)) player.turnLeft(throttle * 0.5f);
		if (inputManager->onKeyAction(GLFW_KEY_RIGHT, GLFW_PRESS)) player.turnRight(throttle * 0.5f);
		if (inputManager->onKeyAction(GLFW_KEY_SPACE, GLFW_PRESS)) player.handbrake();


		if (abs(player.getPosition().z) >= 51.0f || abs(player.getPosition().x) >= 51.0) player.removePhysics();

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		shader.use();

		// OpenGL
		//editorCamera.render();
		playerCamera.setPosition(glm::vec3(player.getPosition().x, player.getPosition().y + 15.0f, player.getPosition().z + 20.0f));
		playerCamera.render();
		plane.render();

		// Physx
		player.render(tires, body);
		obstacle_d.render(ball);
		obstacle_s.render(obstacleMesh);

		glDisable(GL_FRAMEBUFFER_SRGB);
		window.swapBuffers();

	}

	player.free();
	obstacle_d.free();
	obstacle_s.free();
	pm.free();

	glfwTerminate();
	return 0;
}