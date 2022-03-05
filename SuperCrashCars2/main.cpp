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
#include "PowerUp.h"

#include "ImguiManager.h"
#include "AudioManager.h"

int main(int argc, char** argv) {
	Log::info("Starting Game...");

	// OpenGL
	glfwInit();
	Window window(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT, "Super Crash Cars 2");
	std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	window.setCallbacks(inputManager);

	// Shaders
	auto defaultShader = std::make_shared<ShaderProgram>("shaders/shader_vertex.vert", "shaders/shader_fragment.frag");
	auto depthShader = std::make_shared<ShaderProgram>("shaders/simpleDepth.vert", "shaders/simpleDepth.frag");
	Utils::instance().shader = defaultShader;


	// Lighting
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(600.0f, 300.0f, 0.0f);

	// Skybox
	Skybox skybox;

	// Anti-Aliasing (Not working)
	unsigned int samples = 8;
	glfwWindowHint(GLFW_SAMPLES, samples);

	// Camera
	bool cameraToggle = false;
	Camera playerCamera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Camera menuCamera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	playerCamera.setPitch(-30.0f);

	// Physx
	PhysicsManager pm = PhysicsManager(1.5f/60.0f);
	PVehicle enemy = PVehicle(pm, VehicleType::eTOYOTA, PxVec3(1.0f, 30.0f, -10.0f));
	PVehicle player = PVehicle(pm, VehicleType::eTOYOTA, PxVec3(0.0f, 30.0f, 0.0f));

	PowerUp powerUp1 = PowerUp(pm, Model("models/star_powerup/star_powerup.obj"), PowerUpType::eBOOST, PxVec3(-20.0f, 20.0f, -30.0f));
	PowerUp powerUp2 = PowerUp(pm, Model("models/star_powerup/star_powerup.obj"), PowerUpType::eBOOST, PxVec3(163.64, 77.42f + 5.0f, -325.07f));

	std::vector<PVehicle> vehicleList;
	std::vector<PowerUp> powerUps;
	vehicleList.push_back(player);
	vehicleList.push_back(enemy);
	powerUps.push_back(powerUp1);
	powerUps.push_back(powerUp2);
	
	// Controller
	InputController controller1, controller2, controller3, controller4;
	if (glfwJoystickPresent(GLFW_JOYSTICK_1)) controller1 = InputController(GLFW_JOYSTICK_1);
	if (glfwJoystickPresent(GLFW_JOYSTICK_2)) {
		Log::info("Controller 2 connected");
		controller2 = InputController(GLFW_JOYSTICK_2);
	}
	// ImGui 
	ImguiManager imgui(window);

	// Audio 
	AudioManager::get().init();

	// Shadows
	const unsigned int SHADOW_WIDTH = 2048 * 4, SHADOW_HEIGHT = 2048 * 4;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!window.shouldClose() && !Menu::quitGame) {
		
		// always update the time and poll events
		Time::update();
		glfwPollEvents();
		glEnable(GL_DEPTH_TEST);

		// main switch to decide what screen to display
		switch (Menu::screen){
		case Screen::eMAINMENU:
			if (Time::shouldSimulate) {
				Time::startSimTimer();

				// read inputs assume first controller is PS4
				if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
					controller1.PS4InputInMenu();
					//controller.XboxInputInMenu();
				}
				//Second controller is ns
				if (glfwJoystickPresent(GLFW_JOYSTICK_2)) {
					controller2.NSInputInMenu();
					//controller.XboxInputInMenu();
				}
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
				Utils::instance().shader->setVector3("lightPos", lightPos); // later we will change this to the actual light position, leave as this for now
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
					if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
						controller1.PS4InputInMenu();
						//controller.XboxInputInMenu();

					}
				} 
				else {
					Time::startSimTimer();

#pragma region inputs

					if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
						controller1.PS4InputInGame(player);
						//controller.XboxInputInGame(player);
					}

					//if (glfwJoystickPresent(GLFW_JOYSTICK_2)) {
					//	controller2.NSInputInGame(enemy);
					//	//controller.XboxInputInGame(player);
					//}

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
					if (inputManager->onKeyAction(GLFW_KEY_M, GLFW_PRESS)) {
						AudioManager::get().muteToggle();
					}

#pragma endregion

					
					AudioManager::get().setListenerPosition(Utils::instance().pxToGlmVec3(player.getPosition()), player.getFrontVec(), player.getUpVec());

					// applying collisions in main thread instead of collision thread

					for (const PVehicle& car : vehicleList) {
						PVehicle* x = (PVehicle*)car.getRigidDynamic()->userData;
						if (x && x->vehicleAttr.collided) {
							car.getRigidDynamic()->addForce((x->vehicleAttr.forceToAdd), PxForceMode::eIMPULSE);
							x->vehicleAttr.collided = false;
							AudioManager::get().playSound(SFX_CAR_HIT, Utils::instance().pxToGlmVec3(car.getPosition()), 0.5f);
							Log::debug("Player coeff: {}", ((PVehicle*)player.getRigidDynamic()->userData)->vehicleAttr.collisionCoefficient);
							Log::debug("Enemy coeff: {}", ((PVehicle*)enemy.getRigidDynamic()->userData)->vehicleAttr.collisionCoefficient);
						}
					}

					// handling triggers of powerUps in the main thrad instead of the collision thread
					for (const PowerUp& powerUp : powerUps) {
						PowerUp* x = (PowerUp*)powerUp.getRigidStatic()->userData;
						if (x && x->m_triggerEvent.triggered) {
							x->m_triggerEvent.triggered = false;
							x->destroy();
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
					glCullFace(GL_BACK);
					glFrontFace(GL_CCW);
					glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					#pragma region Shadow map

					glCullFace(GL_FRONT);
					glFrontFace(GL_CCW);

					// 1. render depth of scene to texture (from light's perspective)
					// --------------------------------------------------------------
					glm::mat4 lightProjection, lightView;
					glm::mat4 lightSpaceMatrix;
					float near_plane = 1.0f, far_plane = 7.5f;
					//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
					lightProjection = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, 0.1f, 1000.0f);
					lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
					lightSpaceMatrix = lightProjection * lightView;
					// render scene from light's point of view
					

					Utils::instance().shader = depthShader;
					Utils::instance().shader->use();
					Utils::instance().shader->setInt("shadowMap", 1);
					Utils::instance().shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
					Utils::instance().shader->setVector4("lightColor", lightColor);
					//Utils::instance().shader->setVector3("lightPos", Utils::instance().pxToGlmVec3(player.getPosition()));
					Utils::instance().shader->setVector3("lightPos", lightPos);
					Utils::instance().shader->setVector3("camPos", playerCamera.getPosition());


					glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
					glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
					glClear(GL_DEPTH_BUFFER_BIT);
					glActiveTexture(GL_TEXTURE0);
	
					//playerCamera.updateCamera(Utils::instance().pxToGlmVec3(player.getPosition()), player.getFrontVec());
					pm.drawGround();
					enemy.render();
					player.render();
					//skybox.draw(playerCamera.getPerspMat(), glm::mat4(glm::mat3(playerCamera.getViewMat())));

					glBindFramebuffer(GL_FRAMEBUFFER, 0);

					// reset viewport
					glViewport(0, 0, Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					glCullFace(GL_BACK);
					glFrontFace(GL_CCW);
					#pragma endregion 


					Utils::instance().shader = defaultShader;
					Utils::instance().shader->use();
					Utils::instance().shader->setInt("shadowMap", 1);
					Utils::instance().shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
					Utils::instance().shader->setVector4("lightColor", lightColor);
					//Utils::instance().shader->setVector3("lightPos", Utils::instance().pxToGlmVec3(player.getPosition()));
					Utils::instance().shader->setVector3("lightPos", lightPos);
					Utils::instance().shader->setVector3("camPos", playerCamera.getPosition());


					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, depthMap);


					playerCamera.updateCamera(Utils::instance().pxToGlmVec3(player.getPosition()), player.getFrontVec());
					pm.drawGround();
					enemy.render();
					player.render();

					powerUp1.render();
					powerUp2.render();

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

void Render() 
{

}