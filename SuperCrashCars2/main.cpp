#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <chrono>

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

	GLMesh plane(shader, GL_FILL), tires(shader), body(shader), obstacleMesh(shader, GL_FILL);
	plane.createPlane(100, glm::vec3(0.0f));
	tires.createSphere(0.5f, 10, glm::vec3(0.0f, 1.0f, 0.0f));
	body.createCube(0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
	body.scale(glm::vec3(2.0f, 2.0f, 3.0f));
	obstacleMesh.createCube(1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	// Physx
	float throttle = 1.0f;
	PhysicsManager pm(1.0/60.0f);
	PVehicle player(pm);

	PxRigidDynamic* obstacle = pm.createDynamic(PxTransform(player.getRigidDynamic()->getGlobalPose().p + PxVec3(0.0f, 10.0f, -20.0f), PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f))), PxBoxGeometry(1.0f, 1.0f, 1.0f));
	
	// Jump Cooldown and Timer
	bool jumpAvalible = true;
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point coolDownBegin = std::chrono::steady_clock::now();
	

	while (!window.shouldClose()) {
;
		currentTime = std::chrono::steady_clock::now();

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
		if (inputManager->onKeyAction(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_LEFT_SHIFT);
		if (inputManager->onKeyAction(GLFW_KEY_LEFT_CONTROL, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_LEFT_CONTROL);

		// Pressing down now reverses instead of brakes I belive this produces a similar/just as good effect as 
		// witching to reverse under a certain speed with out the complicated-ish and expensive comparisons 
		// but may cause weird behaviour in the future such as backward spining wheels when moving forward. Doesn't 
		// look like this is the case but not sure yet if you want to find do it the other way you prob have to compare every 
		// dimension cause mangnatuide and squareMagnitude wont work easily in this case
		if (inputManager->onKeyAction(GLFW_KEY_UP, GLFW_PRESS)) player.accelerate(throttle);
		if (inputManager->onKeyAction(GLFW_KEY_DOWN, GLFW_PRESS)) player.reverse(throttle * 0.5); 
		if (inputManager->onKeyAction(GLFW_KEY_LEFT, GLFW_PRESS)) player.turnLeft(throttle * 0.5f);
		if (inputManager->onKeyAction(GLFW_KEY_RIGHT, GLFW_PRESS)) player.turnRight(throttle * 0.5f);
		if (inputManager->onKeyAction(GLFW_KEY_SPACE, GLFW_PRESS)) player.handbrake();

		// Prototype jump action, should be move out of main and into PVehicle but I wasn't sure how you wanted to organize that class
		// and if you wanted mulitpule timers/passing timers exetara
		if (inputManager->onKeyAction(GLFW_KEY_E, GLFW_PRESS))
		{
			//player.jump(PxVec3(0.0, 15000.0, 0.0))
			if (std::chrono::duration_cast<std::chrono::microseconds>(currentTime - coolDownBegin).count() > 2000000) // 2 second cooldown !Starts on Cooldown!
			{
				coolDownBegin = std::chrono::steady_clock::now();
				player.getRigidDynamic()->addForce(PxVec3(0.0, 15000.0, 0.0), PxForceMode::eIMPULSE);
			}
		}
		
	
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