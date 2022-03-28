#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "Time.h"

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "Window.h"
#include <cmath>

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

#include "RenderManager.h"

int main(int argc, char** argv) {
	Log::info("Starting Game...");

	// OpenGL
	glfwInit();
	Window window(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT, "Super Crash Cars 2");
	std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	window.setCallbacks(inputManager);

	// Camera
	Camera playerCamera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Camera menuCamera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);

	RenderManager renderer(&window, &playerCamera, &menuCamera);

	// OSCILATION 
	int colorVar = 0;
	double os;

	Time time = Time();



	// In-game UI
	TextRenderer boost(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	boost.Load("freetype/fonts/vemanem.ttf", 50);
	TextRenderer currentPowerup(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	currentPowerup.Load("freetype/fonts/armybuster.ttf", 40);

	// Main Menu Buttons
	TextRenderer menuText(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	menuText.Load("freetype/fonts/bof.ttf", 40);

	static glm::vec3 regCol = glm::vec3(160.f / 255.f, 0.f / 255.f, 75.f / 255.f); // colors
	static glm::vec3 selCol = glm::vec3(222 / 255.f, 70 / 255.f, 80 / 255.f); // 

	std::vector<glm::vec3> buttonColors;
	std::vector<float> menuTextWidth;
	for (int i = 0; i < 5; i++) { 
		buttonColors.push_back(regCol);
		menuTextWidth.push_back(menuText.totalW);
	}

	std::vector<glm::vec3> optionsButtonColors; 

	for (int i = 0; i < 3; i++) {
		optionsButtonColors.push_back(regCol);
	}

	std::vector<glm::vec3> pausedButtonColors;
	std::vector<float> pauseTextWidth;
	for (int i = 0; i < 2; i++) {
		pausedButtonColors.push_back(regCol);
		pauseTextWidth.push_back(menuText.totalW);
	}

	// Anti-Aliasing (Not working)
	unsigned int samples = 8;
	glfwWindowHint(GLFW_SAMPLES, samples);

	// Physx
	PhysicsManager pm = PhysicsManager(1.3f/60.0f);
	PVehicle player = PVehicle(0, pm, VehicleType::eTOYOTA, PxVec3(0.0f, 80.f, 240.0f));
	PVehicle enemy = PVehicle(1, pm, VehicleType::eTOYOTA, PxVec3(0.0f, 80.f, 230.f));
	PVehicle enemy2 = PVehicle(1, pm, VehicleType::eTOYOTA, PxVec3(0.0f, 80.0f, 220.0f));
	PVehicle enemy3 = PVehicle(1, pm, VehicleType::eTOYOTA, PxVec3(0.0f, 80.0f, 210.0f));
	PVehicle enemy4 = PVehicle(1, pm, VehicleType::eTOYOTA, PxVec3(0.0f, 80.0f, 200.0f));

	PowerUp powerUp1 = PowerUp(pm, Model("models/powerups/jump_star/star.obj"), PowerUpType::eJUMP, PxVec3(-120.f, 80.f, 148.0f));
	PowerUp powerUp2 = PowerUp(pm, Model("models/powerups/boost/turbo.obj"), PowerUpType::eBOOST, PxVec3(163.64, 80.f, -325.07f));
	PowerUp powerUp3 = PowerUp(pm, Model("models/powerups/health_star/health.obj"), PowerUpType::eHEALTH, PxVec3(-87.f, 80.f, 182.f));
	PowerUp powerUp4 = PowerUp(pm, Model("models/powerups/jump_star/star.obj"), PowerUpType::eJUMP, PxVec3(-228.f, 80.f, -148.0f));
	PowerUp powerUp5 = PowerUp(pm, Model("models/powerups/shield/shieldman.obj"), PowerUpType::eSHIELD, PxVec3(-130.f, 80.f, -110.f));
	PowerUp powerUp6 = PowerUp(pm, Model("models/powerups/shield/shieldman.obj"), PowerUpType::eSHIELD, PxVec3(28.f, 80.f, -188.0f));
	
	PStatic sphere = PStatic(pm, Model("models/sphere/sphere.obj"), PxVec3(0.f, 80.f, 0.f));

	std::vector<PVehicle*> vehicleList;
	std::vector<PowerUp*> powerUps;
	vehicleList.push_back(&player);
	vehicleList.push_back(&enemy);
	vehicleList.push_back(&enemy2);
	vehicleList.push_back(&enemy3);
	vehicleList.push_back(&enemy4);
	powerUps.push_back(&powerUp1);
	powerUps.push_back(&powerUp2);
	powerUps.push_back(&powerUp3);
	powerUps.push_back(&powerUp4);
	powerUps.push_back(&powerUp5);
	powerUps.push_back(&powerUp6);

	// AI toggle
	bool ai_ON = true;
	
	// Controller
	InputController controller1, controller2, controller3, controller4;
	if (glfwJoystickPresent(GLFW_JOYSTICK_1)) { 
		Log::info("Controller 1 connected");
		controller1 = InputController(GLFW_JOYSTICK_1);
		controller1.connected = true;
	}
	if (glfwJoystickPresent(GLFW_JOYSTICK_2)) {
		Log::info("Controller 2 connected");
		controller2 = InputController(GLFW_JOYSTICK_2);
		controller2.connected = true;
	}
	// ImGui 
	ImguiManager imgui(window);

	// Audio 
	AudioManager::get().init(vehicleList);

	//AudioManager::get().loadCarIdleSound(SFX_CAR_FAST, 0.2f, 0, Utils::instance().pxToGlmVec3(player.getPosition()));
	//AudioManager::get().loadCarIdleSound(SFX_CAR_FAST, 0.2f, 1, Utils::instance().pxToGlmVec3(enemy.getPosition()));

	// Menu
	GameManager::get().initMenu();

	while (!window.shouldClose() && !GameManager::get().quitGame) {
		
		// always update the time and poll events
		time.update();
		glfwPollEvents();
		glEnable(GL_DEPTH_TEST);

		if (time.shouldSimulate) {
			time.startSimTimer();
			AudioManager::get().update();
			// check controller connected; when we have more controllers we will make it into a loop
			// should probably put this away into the controller class
			if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
				if (!controller1.connected) {
					AudioManager::get().playSound(SFX_CONTROLLER_ON, 0.3f);
					controller1.connected = true;
				}
			}
			else {
				if (controller1.connected) {
					AudioManager::get().playSound(SFX_CONTROLLER_OFF, 0.5f);
					controller1.connected = false;
				}

			}

			switch (GameManager::get().screen) {
			case Screen::eMAINMENU: {
				if (controller1.connected) controller1.uniController(false, player);

				break; }
			case Screen::eLOADING: {

				// set up init game here
				time.resetStats();

				for (PVehicle* carPtr : vehicleList) {
					carPtr->m_state = VehicleState::ePLAYING;
					carPtr->m_lives = 3;
					carPtr->vehicleAttr.collisionCoefficient = 0.0f;
					carPtr->m_shieldState = ShieldPowerUpState::eINACTIVE;
					carPtr->m_powerUpPocket = PowerUpType::eEMPTY;
					carPtr->reset();
				}
				for (PowerUp* powerUpPtr : powerUps) {
					powerUpPtr->forceRespawn();
				}

				GameManager::get().screen = Screen::ePLAYING;

				break; }
			case Screen::ePLAYING: {

				if (GameManager::get().paused) { // paused, read the inputs using the menu function
					if (controller1.connected) controller1.uniController(false, player);
				}
				else { // in game

					if (controller1.connected) controller1.uniController(true, player);
					AudioManager::get().setListenerPosition(Utils::instance().pxToGlmVec3(player.getPosition()), player.getFrontVec(), player.getUpVec());

					for (PVehicle* carPtr : vehicleList) {
						if (carPtr->vehicleAttr.collided) {
							carPtr->getRigidDynamic()->addForce((carPtr->vehicleAttr.forceToAdd), PxForceMode::eIMPULSE);
							carPtr->getRigidDynamic()->addForce(PxVec3(0.f, 10.f + 5.f * carPtr->vehicleAttr.collisionCoefficient, 0.f ), PxForceMode::eVELOCITY_CHANGE);
							carPtr->flashWhite();
							carPtr->vehicleAttr.collided = false;
							AudioManager::get().playSound(SFX_CAR_HIT, Utils::instance().pxToGlmVec3(carPtr->vehicleAttr.collisionMidpoint), 0.3f);
						}

						if (carPtr->vehicleAttr.reachedTarget && carPtr->carid == 1) {
							int rndIndex = Utils::instance().random(0, (int)vehicleList.size() - 1);
							if (vehicleList[rndIndex] != carPtr) {
								carPtr->vehicleAttr.reachedTarget = false;
								carPtr->chaseVehicle(*vehicleList[rndIndex]);
							}
						}

						carPtr->updateState(); // to check for car death
					}

					for (PowerUp* powerUpPtr : powerUps) {
						if (!powerUpPtr->active) {
							powerUpPtr->tryRespawn();
						}
						else if (powerUpPtr->triggered) {
							AudioManager::get().playSound(SFX_ITEM_COLLECT, Utils::instance().pxToGlmVec3(powerUpPtr->getPosition()), 0.3f);
							powerUpPtr->collect();
						}
					}

				

					if (ai_ON) {
						for (int i = 0; i < vehicleList.size(); i++) {
							if (vehicleList[i]->carid != 1) continue;
							PVehicle* targetVehicle = (PVehicle*)vehicleList[i]->vehicleAttr.targetVehicle;
							if (targetVehicle) vehicleList[i]->chaseVehicle(*targetVehicle);
							else {
								int rndIndex = Utils::instance().random(0, (int)vehicleList.size() - 1);
								if (vehicleList[rndIndex] != vehicleList[i]) {
									vehicleList[i]->chaseVehicle(*vehicleList[rndIndex]);
								}
							}
						}
					}
					pm.simulate();

					for (PVehicle* vehicle : vehicleList) vehicle->updatePhysics();

					time.endSimTimer();
				}

				break; }
			case Screen::eGAMEOVER:{
				if (controller1.connected) controller1.uniController(false, player);
				break; }
			}
			time.endSimTimer(); // end sim timer !
		}

		if (time.shouldRender) { 
			time.startRenderTimer();
			
			renderer.startFrame();
				
			switch (GameManager::get().screen) {
			case Screen::eMAINMENU: {
				renderer.skybox.draw(menuCamera.getPerspMat(), glm::mat4(glm::mat3(menuCamera.getViewMat())));

				switch (GameManager::get().mainMenuScreen){
				case MainMenuScreen::eMAIN_SCREEN:

					// Menu rendering
					for (int i = 0; i < 5; i++) {
						if ((int)GameManager::get().menuButton == i) buttonColors.at(i) = selCol;
						else buttonColors.at(i) = regCol;
					}
					menuText.RenderText("START", Utils::instance().SCREEN_WIDTH / 2 - (menuTextWidth.at(0) / 2), 100.f, 1.0f, buttonColors.at(0));
					menuTextWidth.at(0) = menuText.totalW;
					menuText.RenderText("HOW TO PLAY", Utils::instance().SCREEN_WIDTH / 2 - (menuTextWidth.at(1) / 2), 200.f, 1.0f, buttonColors.at(1));
					menuTextWidth.at(1) = menuText.totalW;
					menuText.RenderText("OPTIONS", Utils::instance().SCREEN_WIDTH / 2 - (menuTextWidth.at(2) / 2), 300.f, 1.0f, buttonColors.at(2));
					menuTextWidth.at(2) = menuText.totalW;
					menuText.RenderText("CREDITS", Utils::instance().SCREEN_WIDTH / 2 - (menuTextWidth.at(3) / 2), 400.f, 1.0f, buttonColors.at(3));
					menuTextWidth.at(3) = menuText.totalW;
					menuText.RenderText("QUIT", Utils::instance().SCREEN_WIDTH / 2 - (menuTextWidth.at(4) / 2), 500.f, 1.0f, buttonColors.at(4));
					menuTextWidth.at(4) = menuText.totalW;


					break;
				case MainMenuScreen::eHOWTOPLAY_SCREEN:

					menuText.RenderText("This is how to play", Utils::instance().SCREEN_WIDTH / 3, 500.f, 1.0f, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));

					break;
				case MainMenuScreen::eOPTIONS_SCREEN:

					for (int i = 0; i < 3; i++) {
						if ((int)GameManager::get().optionsButton == i) optionsButtonColors.at(i) = selCol;
						else optionsButtonColors.at(i) = regCol;
					}

					menuText.RenderText("BGM: " + std::to_string(AudioManager::get().getBGMLevel()) , Utils::instance().SCREEN_WIDTH / 4 , 100.f, 1.0f, optionsButtonColors.at(0));
					menuText.RenderText("SFX: " + std::to_string(AudioManager::get().getSFXLevel()), Utils::instance().SCREEN_WIDTH / 4, 200.f, 1.0f, optionsButtonColors.at(1));
					menuText.RenderText("BACK", Utils::instance().SCREEN_WIDTH / 4 , 300.f, 1.0f, optionsButtonColors.at(2));


					break;
				case MainMenuScreen::eCREDITS_SCREEN:

					menuText.RenderText("Haha credits", Utils::instance().SCREEN_WIDTH / 3, 500.f, 1.0f, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));

					break;
				}




				// imGUI section
				imgui.initFrame();
				imgui.renderMenu(ai_ON);
				imgui.endFrame();

				break; }
			case Screen::eLOADING: {

				renderer.skybox.draw(menuCamera.getPerspMat(), glm::mat4(glm::mat3(menuCamera.getViewMat())));

				// imGUI section
				imgui.initFrame();
				imgui.renderMenu(ai_ON);
				imgui.endFrame();

				break; }

			case Screen::ePLAYING:

				os = (sin((float)colorVar / 20) + 1.0) / 2.0;
				colorVar++;
				
				playerCamera.UpdateCameraPosition(Utils::instance().pxToGlmVec3(player.getPosition()), player.getFrontVec()); // only move cam once.
				playerCamera.UpdateCameraPosition(Utils::instance().pxToGlmVec3(player.getPosition()), player.getFrontVec()); // only move cam once.

				renderer.renderShadows(vehicleList, powerUps);

				renderer.skybox.draw(playerCamera.getPerspMat(), glm::mat4(glm::mat3(playerCamera.getViewMat())));

				renderer.renderCars(vehicleList);
				renderer.renderPowerUps(powerUps, os);

				renderer.renderNormalObjects(); // prepare to draw NORMAL objects, doesn't actually render anything.
				pm.drawGround();

				renderer.renderTransparentObjects(vehicleList, sphere, os, time);

				if (GameManager::get().paused) {
					// if game is paused, we will render an overlay.
					// render the PAUSE MENU HERE


					for (int i = 0; i < 2; i++) {
						if ((int)GameManager::get().pauseButton == i) pausedButtonColors.at(i) = selCol;
						else pausedButtonColors.at(i) = regCol;
					}


					menuText.RenderText("PAUSED", Utils::instance().SCREEN_WIDTH / 5.0f, 75.f, 1.0f, glm::vec3(0.992f, 0.164f, 0.129f));

					menuText.RenderText("RESUME", Utils::instance().SCREEN_WIDTH / 2 - (pauseTextWidth.at(0) / 2), 300.f, 1.0f, pausedButtonColors.at(0));
					pauseTextWidth.at(0) = menuText.totalW;
					menuText.RenderText("QUIT", Utils::instance().SCREEN_WIDTH / 2 - (pauseTextWidth.at(0) / 2), 400.f, 1.0f, pausedButtonColors.at(1));
					pauseTextWidth.at(1) = menuText.totalW;
					
				}

				// imgui
				imgui.initFrame();
				imgui.renderStats(player, time.averageSimTime, time.averageRenderTime);
				imgui.renderDamageHUD(vehicleList);
				imgui.renderMenu(ai_ON);
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
					currentPowerup.RenderText("Pocket: Empty", Utils::instance().SCREEN_WIDTH / 2 - (currentPowerup.totalW / 2), 10.f, 1.0f, glm::vec3(0.478f, 0.003f, 0.f));
					break;
				case PowerUpType::eJUMP:
					currentPowerup.RenderText("Pocket: Jump", Utils::instance().SCREEN_WIDTH / 2 - (currentPowerup.totalW / 2), 10.f, 1.0f, glm::vec3(1.f, 0.050f, 0.039f));
					break;

				case PowerUpType::eSHIELD:
					currentPowerup.RenderText("Pocket: Shield", Utils::instance().SCREEN_WIDTH / 2 - (currentPowerup.totalW / 2), 10.f, 1.0f, glm::vec3(1.f, 0.050f, 0.039f));
					break;

				}

				break;
			case Screen::eGAMEOVER:

				renderer.skybox.draw(menuCamera.getPerspMat(), glm::mat4(glm::mat3(menuCamera.getViewMat())));
				menuText.RenderText("Game Over", Utils::instance().SCREEN_WIDTH / 3, 200, 1.0f, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));
				menuText.RenderText("Player " + std::to_string(GameManager::get().winner + 1 ) + " wins", Utils::instance().SCREEN_WIDTH / 3, 300, 1.0f, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));
				
				menuText.RenderText("QUIT ", Utils::instance().SCREEN_WIDTH / 3, 400, 1.0f, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));

				imgui.initFrame();
				imgui.renderMenu(ai_ON);
				imgui.endFrame();

				break;
			}

			renderer.endFrame();
			time.endRenderTimer();
		}

	}

	player.free();
	enemy.free();
	pm.free();
	imgui.freeImgui();

	glfwTerminate();
	return 0;
}