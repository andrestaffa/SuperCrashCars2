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

#include "PhysicsManager.h"
#include "PVehicle.h"

/* TODO:
		- Create PActor class which all classes with the prefix "P" will inherit from.
		- Improve PVehicle class (ex. getTransform(), getPosition(), etc...).
		- Create PDyanmic, PStatic classes.
		- Implement player camera.
		- Start implementation of Open Asset Import Library.
		- Turn off collision when off plane. (gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, pm.gAllocator))
*/

//test of git

int main(int argc, char** argv) {
	Log::info("Starting Game...");

	// OpenGL
	glfwInit();
	Window window(Utils::shared().SCREEN_WIDTH, Utils::shared().SCREEN_HEIGHT, "Super Crash Cars 2");
	GLDebug::enable();
	std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>(Utils::shared().SCREEN_WIDTH, Utils::shared().SCREEN_HEIGHT);
	window.setCallbacks(inputManager);
	ShaderProgram shader("shaders/shader_vertex.vert", "shaders/shader_fragment.frag");
	Camera editorCamera(shader, Utils::shared().SCREEN_WIDTH, Utils::shared().SCREEN_HEIGHT, glm::vec3(-2.0f, 4.0f, 10.0f));

	GLMesh plane(shader, GL_FILL), tires(shader), body(shader), obstacleMesh(shader, GL_LINES);
	plane.createPlane(100, glm::vec3(0.0f));
	tires.createSphere(0.5f, 10, glm::vec3(0.0f, 1.0f, 0.0f));
	body.createCube(0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
	body.scale(glm::vec3(2.0f, 2.0f, 3.0f));
	obstacleMesh.createCube(1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	// Physx
	float throttle = 1.0f;
	PhysicsManager pm(1.0/100.0f);
	PVehicle player(pm);

	PxRigidDynamic* obstacle = pm.createDynamic(PxTransform(player.getRigidDynamic()->getGlobalPose().p + PxVec3(0.0f, 10.0f, -20.0f), PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f))), PxBoxGeometry(1.0f, 1.0f, 1.0f));
	
	while (!window.shouldClose()) {
;
		pm.simulate();
		player.stepPhysics();

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

		PxVec3 playerPos = player.getRigidDynamic()->getGlobalPose().p;
		if (abs(playerPos.z) >= 51.0f || abs(playerPos.x) >= 51.0) player.removePhysics();

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		shader.use();
		editorCamera.render();

		plane.render();
		player.render(tires, body);

		pm.renderActor(*obstacle, obstacleMesh);

		glDisable(GL_FRAMEBUFFER_SRGB);
		window.swapBuffers();

	}

	player.cleanupVehicle();
	pm.cleanupPhysics();

	glfwTerminate();
	return 0;
}