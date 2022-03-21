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
#include "TextRenderer.h"

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
	auto carShader = std::make_shared<ShaderProgram>("shaders/car.vert", "shaders/car.frag");
	Utils::instance().shader = defaultShader;


	// Lighting
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(300.0f, 400.0f, 0.0f);

	// Skybox
	Skybox skybox;

	// In-game UI
	TextRenderer boost(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	boost.Load("freetype/fonts/vemanem.ttf", 50);
	TextRenderer currentPowerup(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	currentPowerup.Load("freetype/fonts/armybuster.ttf", 40);

	// Main Menu Buttons
	TextRenderer menuText(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	menuText.Load("freetype/fonts/bof.ttf", 40);

	static glm::vec3 regCol = glm::vec3(0.f, 0.478f, 0.066f); // colors
	static glm::vec3 selCol = glm::vec3(0.478f, 0.113f, 0.f); // 

	std::vector<glm::vec3> buttonColors;
	std::vector<float> menuTextWidth;
	for (int i = 0; i < 4; i++) { 
		buttonColors.push_back(regCol);
		menuTextWidth.push_back(menuText.totalW);
	}

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
	PVehicle enemy = PVehicle(1, pm, VehicleType::eTOYOTA, PxVec3(-150.f, 100.f, -150.f));
	PVehicle player = PVehicle(2, pm, VehicleType::eTOYOTA, PxVec3(0.0f, 100.0f, 240.0f));

	PowerUp powerUp1 = PowerUp(pm, Model("models/powerups/jump_star/star.obj"), PowerUpType::eJUMP, PxVec3(-120.f, 100.0f, 148.0f));
	PowerUp powerUp2 = PowerUp(pm, Model("models/powerups/boost/turbo.obj"), PowerUpType::eBOOST, PxVec3(163.64, 77.42f + 5.0f, -325.07f));
	PowerUp powerUp3 = PowerUp(pm, Model("models/powerups/health_star/health.obj"), PowerUpType::eHEALTH, PxVec3(-87.f, 100.f, 182.f));
	PowerUp powerUp4 = PowerUp(pm, Model("models/powerups/jump_star/star.obj"), PowerUpType::eJUMP, PxVec3(228.f, 100.0f, -148.0f));
	
	std::vector<PVehicle*> vehicleList;
	std::vector<PowerUp*> powerUps;
	vehicleList.push_back(&player);
	vehicleList.push_back(&enemy);
	powerUps.push_back(&powerUp1);
	powerUps.push_back(&powerUp2);
	powerUps.push_back(&powerUp3);
	powerUps.push_back(&powerUp4);


	// AI toggle
	bool ai_ON;
	
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

	// Menu
	GameManager::get().initMenu();

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

	while (!window.shouldClose() && !GameManager::get().quitGame) {
		
		// always update the time and poll events
		Time::update();
		glfwPollEvents();
		glEnable(GL_DEPTH_TEST);

		// main switch to decide what screen to display
		switch (GameManager::get().screen){
		case Screen::eMAINMENU:
			if (Time::shouldSimulate) {
				Time::startSimTimer();

				// read inputs 
				if (glfwJoystickPresent(GLFW_JOYSTICK_1)) controller1.uniController(false, player);
				
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
			

				skybox.draw(menuCamera.getPerspMat(), glm::mat4(glm::mat3(menuCamera.getViewMat())));

				// Menu rendering

				for (int i = 0; i < 4; i++) {
				//	buttonColors.push_back(regCol);
				//	menuTextWidth.push_back(menuText.totalW);
					if ((int)GameManager::get().menuButton == i) buttonColors.at(i) = selCol;
					else buttonColors.at(i) = regCol;

				}

				menuText.RenderText("START", Utils::instance().SCREEN_WIDTH/2 - (menuTextWidth.at(0) / 2), 100.f, 1.0f, buttonColors.at(0));
				menuTextWidth.at(0) = menuText.totalW;
				menuText.RenderText("HOW TO PLAY", Utils::instance().SCREEN_WIDTH/2 - (menuTextWidth.at(1) / 2), 200.f, 1.0f, buttonColors.at(1));
				menuTextWidth.at(1) = menuText.totalW;
				menuText.RenderText("CREDITS", Utils::instance().SCREEN_WIDTH/2 - (menuTextWidth.at(2) / 2),300.f, 1.0f, buttonColors.at(2));
				menuTextWidth.at(2) = menuText.totalW;
				menuText.RenderText("QUIT", Utils::instance().SCREEN_WIDTH/2 - (menuTextWidth.at(3) / 2),400.f, 1.0f, buttonColors.at(3));
				menuTextWidth.at(3) = menuText.totalW;

				// imGUI section
				imgui.initFrame();
				imgui.renderMenu(ai_ON);
				imgui.endFrame();


				glDisable(GL_FRAMEBUFFER_SRGB);
				window.swapBuffers();
				Time::renderFrame(); // turn off the render flag and stop timer
			}
			break;
		case Screen::eLOADING: 
			Time::startRenderTimer();
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glFrontFace(GL_CW);
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Utils::instance().shader->use();
			Utils::instance().shader->setVector4("lightColor", lightColor);
			Utils::instance().shader->setVector3("lightPos", lightPos);
			Utils::instance().shader->setVector3("camPos", menuCamera.getPosition());


			skybox.draw(menuCamera.getPerspMat(), glm::mat4(glm::mat3(menuCamera.getViewMat())));

			// imGUI section
			imgui.initFrame();
			imgui.renderMenu(ai_ON);
			imgui.endFrame();


			glDisable(GL_FRAMEBUFFER_SRGB);
			window.swapBuffers();
			Time::renderFrame(); // turn off the render flag and stop timer

			// set up init game here
			Time::resetStats();

			for (PVehicle* carPtr : vehicleList) {
				carPtr->m_state = VehicleState::ePLAYING;
				carPtr->m_lives = 100;
				carPtr->vehicleAttr.collisionCoefficient = 0.0f;
				carPtr->reset();
			}

			for (PowerUp* powerUpPtr : powerUps) {
				powerUpPtr->forceRespawn();
			}




			GameManager::get().screen = Screen::ePLAYING;

			break;
		case Screen::ePLAYING:
			// simulate when unpaused, otherwise just grab the inputs.
			if (Time::shouldSimulate) {

				if (GameManager::get().paused) { // paused, read the inputs using the menu function
					if (glfwJoystickPresent(GLFW_JOYSTICK_1)) controller1.uniController(false, player);
				} 
				else {
					Time::startSimTimer();

#pragma region inputs

					if (glfwJoystickPresent(GLFW_JOYSTICK_1)) controller1.uniController(true, player);

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
					if (inputManager->onKeyAction(GLFW_KEY_R, GLFW_PRESS)) {
						player.reset();
						enemy.reset();
						Time::resetStats();
					}

#pragma endregion

					AudioManager::get().setListenerPosition(Utils::instance().pxToGlmVec3(player.getPosition()), player.getFrontVec(), player.getUpVec());

					for (PVehicle* carPtr : vehicleList) {
						if (carPtr->vehicleAttr.collided) {
							carPtr->getRigidDynamic()->addForce((carPtr->vehicleAttr.forceToAdd), PxForceMode::eIMPULSE);
							carPtr->flashWhite();
							carPtr->vehicleAttr.collided = false;
							AudioManager::get().playSound(SFX_CAR_HIT, Utils::instance().pxToGlmVec3(carPtr->vehicleAttr.collisionMidpoint), 0.4f);
						}
						carPtr->updateState();
					}

					for (PowerUp* powerUpPtr : powerUps) {
						if (!powerUpPtr->active) {
							powerUpPtr->tryRespawn();
						}
						else if (powerUpPtr->triggered) {
							AudioManager::get().playSound(SFX_ITEM_COLLECT, Utils::instance().pxToGlmVec3(powerUpPtr->getPosition()), 0.65f);
							powerUpPtr->collect();
						}
					}

					if (ai_ON) enemy.chaseVehicle(player);
					pm.simulate();
					
					player.updatePhysics();
					enemy.updatePhysics();
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
					lightProjection = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, 0.1f, 1500.0f);
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
	
					for (PVehicle* carPtr : vehicleList) {
						carPtr->render();
					}

					for (PowerUp* powerUpPtr : powerUps) {
						if (powerUpPtr->active) {
							powerUpPtr->render();
						}
					}


					glBindFramebuffer(GL_FRAMEBUFFER, 0);

					// reset viewport
					glViewport(0, 0, Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					glCullFace(GL_BACK);
					glFrontFace(GL_CCW);
					#pragma endregion 


					// Cars rendering
					Utils::instance().shader = carShader;
					Utils::instance().shader->use();
					Utils::instance().shader->setInt("shadowMap", 1);
					Utils::instance().shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
					Utils::instance().shader->setVector4("lightColor", lightColor);
					//Utils::instance().shader->setVector3("lightPos", Utils::instance().pxToGlmVec3(player.getPosition()));
					Utils::instance().shader->setVector3("lightPos", lightPos);
					Utils::instance().shader->setVector3("camPos", playerCamera.getPosition());

					playerCamera.updateCamera(Utils::instance().pxToGlmVec3(player.getPosition()), player.getFrontVec());

					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, depthMap);

					for (PVehicle* carPtr : vehicleList) {
						Utils::instance().shader->setFloat("damage", carPtr->vehicleAttr.collisionCoefficient * 0.3); // number is how fast car turns red
						Utils::instance().shader->setFloat("flashStrength", carPtr->vehicleParams.flashWhite);
						carPtr->render();
					}


					// Other rendering
					Utils::instance().shader = defaultShader;
					Utils::instance().shader->use();
					Utils::instance().shader->setInt("shadowMap", 1);
					Utils::instance().shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
					Utils::instance().shader->setVector4("lightColor", lightColor);
					Utils::instance().shader->setVector3("lightPos", lightPos);
					Utils::instance().shader->setVector3("camPos", playerCamera.getPosition());

					playerCamera.updateCamera(Utils::instance().pxToGlmVec3(player.getPosition()), player.getFrontVec());

					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, depthMap);

					pm.drawGround();
					for (PowerUp* powerUpPtr : powerUps) {
						if (powerUpPtr->active) {
							powerUpPtr->render();
						}
					}

					skybox.draw(playerCamera.getPerspMat(), glm::mat4(glm::mat3(playerCamera.getViewMat())));

					if (GameManager::get().paused) {
						// if game is paused, we will render an overlay.
						// render the PAUSE MENU HERE
					}

					// imgui
					imgui.initFrame();
					imgui.renderStats(player);
					imgui.renderDamageHUD(vehicleList);
					imgui.renderMenu(ai_ON);
					//imgui.renderPlayerHUD(player);
					//imgui.renderSliders(player, enemy);
					imgui.endFrame();


					// Damage and live indicator in progress
					for (PVehicle* carPtr : vehicleList) {
						float colCoef = carPtr->vehicleAttr.collisionCoefficient;
					}
					for (PVehicle* carPtr : vehicleList) {
						std::string lives = std::to_string(carPtr->m_lives);
					}

					// Boost and Powerup indicator

					boost.RenderText(std::to_string(player.vehicleParams.boost), 10.f, Utils::instance().SCREEN_HEIGHT - 50.f, 1.0f, glm::vec3(0.992f, 0.164f, 0.129f));
					switch (player.getPocket()) {
					case PowerUpType::eEMPTY:
						currentPowerup.RenderText("Pocket: Empty", Utils::instance().SCREEN_WIDTH / 2 - (currentPowerup.totalW/2), 10.f, 1.0f, glm::vec3(0.478f, 0.003f, 0.f));
						break;
					case PowerUpType::eJUMP:
						currentPowerup.RenderText("Pocket: Jump", Utils::instance().SCREEN_WIDTH / 2 - (currentPowerup.totalW / 2), 10.f, 1.0f, glm::vec3(1.f, 0.050f, 0.039f));
						break;

					case PowerUpType::eSHIELD:
						currentPowerup.RenderText("Pocket: Shield", Utils::instance().SCREEN_WIDTH / 2 - (currentPowerup.totalW / 2), 10.f, 1.0f, glm::vec3(1.f, 0.050f, 0.039f));
						break;

					}



					glDisable(GL_FRAMEBUFFER_SRGB);
					window.swapBuffers();

					Time::renderFrame(); // turn off the render flag and stop timer

				}
			break;
		case Screen::eGAMEOVER:
			if (Time::shouldSimulate) {
				Time::startSimTimer();

				// read inputs 
				if (glfwJoystickPresent(GLFW_JOYSTICK_1)) controller1.uniController(false, player);

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
				Utils::instance().shader->setVector3("lightPos", lightPos); 
				Utils::instance().shader->setVector3("camPos", menuCamera.getPosition());


				skybox.draw(menuCamera.getPerspMat(), glm::mat4(glm::mat3(menuCamera.getViewMat())));

				// imGUI section
				imgui.initFrame();
				imgui.renderMenu(ai_ON);
				imgui.endFrame();


				glDisable(GL_FRAMEBUFFER_SRGB);
				window.swapBuffers();
				Time::renderFrame(); // turn off the render flag and stop timer
			}
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
