#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "Window.h"

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/type_ptr.hpp"

#include "InputManager.h"
#include "InputController.h"

#include "Camera.h"
#include "Skybox.h"

#include "PVehicle.h"
#include "PDynamic.h"
#include "PStatic.h"

#include "ImguiManager.h"

int main(int argc, char** argv) {
	Log::info("Starting Game...");

	// OpenGL
	glfwInit();
	Window window(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT, "Super Crash Cars 2");
	
	Utils::instance().shader = std::make_shared<ShaderProgram>("shaders/shader_vertex.vert", "shaders/shader_fragment.frag");

	std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	window.setCallbacks(inputManager);

	// Lighting
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// skybox
	Skybox skybox;

	// Anti-Aliasing not sure if this works rn becuase doesn't work for frame buffer, but we are missing some parts of frame buffer if we use it can't tell
	unsigned int samples = 8;
	glfwWindowHint(GLFW_SAMPLES, samples);
	glEnable(GL_DEPTH_TEST);

	// Camera
	bool cameraToggle = false;
	Camera playerCamera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Camera menuCamera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	playerCamera.setPitch(-30.0f);

	// Physx
	PhysicsManager pm = PhysicsManager(1.5f/60.0f);
	PVehicle enemy = PVehicle(pm, VehicleType::eTOYOTA, PxVec3(1.0f, 30.0f, -10.0f));
	PVehicle player = PVehicle(pm, VehicleType::eTOYOTA, PxVec3(0.0f, 30.0f, 0.0f));

	std::vector<PVehicle> vehicleList;
	vehicleList.push_back(player);
	vehicleList.push_back(enemy);
	
	// Controller
	InputController controller;
	if (glfwJoystickPresent(GLFW_JOYSTICK_1)) controller = InputController(GLFW_JOYSTICK_1);

	// ImGui 
	ImguiManager imgui(window);

	while (!window.shouldClose() && !Menu::quitGame) {
		
		// always update the time and poll events
		Time::update();
		glfwPollEvents();

		// main switch to decide what screen to display
		switch (Menu::screen){
		case Screen::eMAINMENU:
			if (Time::shouldSimulate) {
				Time::startSimTimer();
				// read inputs
				controller.PS4InputInMenu();
				//controller.XboxInputInMenu();
				Time::simulatePhysics(); // not technically physics but we reset the bool + timer here
			}
			if (Time::shouldRender) { // render at 60fps even in menu
				Time::startRenderTimer();
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				glFrontFace(GL_CW);
				glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				Utils::instance().shader->use();
				Utils::instance().shader->setVector4("lightColor", lightColor);
				Utils::instance().shader->setVector3("lightPos", menuCamera.getPosition()); // later we will change this to the actual light position, leave as this for now
				Utils::instance().shader->setVector3("camPos", menuCamera.getPosition());

				skybox.draw(menuCamera.getPerspMat(), glm::mat4(glm::mat3(menuCamera.getViewMat())));

				// imGUI section
				imgui.initFrame();
				imgui.renderMenu();
				imgui.endFrame();


				glDisable(GL_FRAMEBUFFER_SRGB);
				window.swapBuffers();
				Time::renderFrame(); // turn off the render flag and stop timer
			}
			break;
		case Screen::eLOADING: // not used rn 
			// if we ever have a loading screen (maybe)
			break;
		case Screen::ePLAYING:
			// simulate when unpaused, otherwise just grab the inputs.
			if (Time::shouldSimulate) {
				if (Menu::paused) { // paused, read the inputs using the menu function
					controller.PS4InputInMenu();
					//controller.XboxInputInMenu();
				} 
				else {
					Time::startSimTimer();
#pragma region inputs

					if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
						controller.PS4InputInGame(player);
						//controller.XboxInputInGame(player);
					}

					if (inputManager->onKeyAction(GLFW_KEY_UP, GLFW_PRESS)) player.accelerate(player.vehicleParams.k_throttle);
					if (inputManager->onKeyAction(GLFW_KEY_DOWN, GLFW_PRESS)) player.reverse(player.vehicleParams.k_throttle * 0.5f);
					if (inputManager->onKeyAction(GLFW_KEY_LEFT, GLFW_PRESS)) player.turnLeft(player.vehicleParams.k_throttle * 0.5f);
					if (inputManager->onKeyAction(GLFW_KEY_RIGHT, GLFW_PRESS)) player.turnRight(player.vehicleParams.k_throttle * 0.5f);
					if (inputManager->onKeyAction(GLFW_KEY_SPACE, GLFW_PRESS)) {
						player.handbrake();
						Time::resetStats();
					}
					if (inputManager->onKeyAction(GLFW_KEY_E, GLFW_PRESS)) player.jump();
					if (inputManager->onKeyAction(GLFW_KEY_F, GLFW_PRESS)) player.boost();
					if (inputManager->onKeyAction(GLFW_KEY_R, GLFW_PRESS) || Menu::startFlag == true) {
						player.reset();
						enemy.reset();
						Menu::startFlag = false;
					}

#pragma endregion

					// applying collisions in main thread instead of collision thread
					for (PVehicle car : vehicleList) {
						VehicleCollisionAttributes* x = (VehicleCollisionAttributes*)car.getRigidDynamic()->userData;
						if (x && x->collided) {
							car.getRigidDynamic()->addForce((x->forceToAdd), PxForceMode::eIMPULSE);
							x->collided = false;

							Log::debug("Player coeff: {}", ((VehicleCollisionAttributes*)player.getRigidDynamic()->userData)->collisionCoefficient);
							Log::debug("Enemy coeff: {}", ((VehicleCollisionAttributes*)enemy.getRigidDynamic()->userData)->collisionCoefficient);
						}
					}
					
					pm.simulate();
					player.update();
					enemy.update();
					Time::simulatePhysics();
				}
			}
			if (Time::shouldRender) {
					Time::startRenderTimer();

					glEnable(GL_CULL_FACE);
					glCullFace(GL_FRONT);
					glFrontFace(GL_CW);
					glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					Utils::instance().shader->use();
					Utils::instance().shader->setVector4("lightColor", lightColor);
					Utils::instance().shader->setVector3("lightPos", Utils::instance().pxToGlmVec3(player.getPosition()));
					Utils::instance().shader->setVector3("camPos", playerCamera.getPosition());

					playerCamera.updateCamera(Utils::instance().pxToGlmVec3(player.getPosition()), player.getFrontVec());

					pm.drawGround();
					enemy.render();
					player.render();

					skybox.draw(playerCamera.getPerspMat(), glm::mat4(glm::mat3(playerCamera.getViewMat())));

					if (Menu::paused) {
						// if game is paused, we will render an overlay.
						// render the PAUSE MENU HERE
					}

					// imgui
					imgui.initFrame();
					imgui.renderStats(player);
					imgui.renderSliders(player, enemy);
					imgui.renderMenu();
					imgui.endFrame();

					glDisable(GL_FRAMEBUFFER_SRGB);
					window.swapBuffers();

					Time::renderFrame(); // turn off the render flag and stop timer

				}
			break;
		case Screen::eGAMEOVER:
			// gome over not implemented yet
			break;
		}
	}

	player.free();
	enemy.free();
	pm.free();
	imgui.freeImgui();

	glfwTerminate();
	return 0;
}

