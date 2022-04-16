﻿#include <GL/glew.h>
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
#include "Texture.h"
#include "Image.h"

#include "PVehicle.h"
#include "PDynamic.h"
#include "PStatic.h"
#include "PowerUp.h"

#include "ImguiManager.h"
#include "AudioManager.h"

#include "RenderManager.h"
#include "MiniMap.h"



int main(int argc, char** argv) {
	Log::info("Starting Game...");

	// OpenGL
	glfwInit();
	//Window window(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT, "Super Crash Cars 2");
	Window window(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT, "Super Crash Cars 2", glfwGetPrimaryMonitor(), NULL);

	std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	window.setCallbacks(inputManager);

	// Camera
	Camera p1Camera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Camera p2Camera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Camera p3Camera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Camera p4Camera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	std::vector<Camera*> cameraList;
	cameraList.push_back(&p1Camera);
	cameraList.push_back(&p2Camera);
	cameraList.push_back(&p3Camera);
	cameraList.push_back(&p4Camera);

	Camera menuCamera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	menuCamera.setPosition(glm::vec3(-232.f, 307.f, 387.f));
	menuCamera.setPitch(-20.f);
	menuCamera.setYaw(-170.f);
	menuCamera.UpdateVP();
	cameraList.push_back(&menuCamera);

	RenderManager renderer(&window, &cameraList, &menuCamera);

	// OSCILATION
	int colorVar = 0;
	double os;

	Time time = Time();

	// In-game UI

	TextRenderer currentPowerup(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	currentPowerup.Load("freetype/fonts/poppins.ttf", 40);



	// Image rendering
	std::vector<Image*> imageList;
	Image mainMenu = Image(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Image image1 = Image(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Image image2 = Image(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Image image3 = Image(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Image image4 = Image(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Image e0 = Image(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Image e1 = Image(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Image e2 = Image(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Image e3 = Image(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Image e4 = Image(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);

	imageList.push_back(&e0);
	imageList.push_back(&e1);
	imageList.push_back(&e2);
	imageList.push_back(&e3);
	imageList.push_back(&e4);
	Texture menu("textures/scc2.png", GL_LINEAR);
	Texture texture("textures/htp.png", GL_LINEAR);
	Texture con("textures/controller.png", GL_LINEAR);
	Texture star("textures/star.png", GL_LINEAR);
	Texture shield("textures/shield.png", GL_LINEAR);


	// Main Menu Buttons
	TextRenderer menuText(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	menuText.Load("freetype/fonts/poppins.ttf", 40);

	static glm::vec3 regCol = glm::vec3(160.f / 255.f, 0.f / 255.f, 75.f / 255.f); // colors
	static glm::vec3 selCol = glm::vec3(222 / 255.f, 70 / 255.f, 80 / 255.f); //

	std::vector<glm::vec3> buttonColors;
	std::vector<float> menuTextWidth;
	for (int i = 0; i < 6; i++) {
		buttonColors.push_back(regCol);
		menuTextWidth.push_back(menuText.totalW);
	}

	std::vector<glm::vec3> playerSelectButtonColors;
	for (int i = 0; i < 2; i++) {
		playerSelectButtonColors.push_back(regCol);
	}

	std::vector<glm::vec3> optionsButtonColors;
	for (int i = 0; i < 4; i++) {
		optionsButtonColors.push_back(regCol);
	}

	std::vector<glm::vec3> pausedButtonColors;
	std::vector<float> pauseTextWidth;
	for (int i = 0; i < 2; i++) {
		pausedButtonColors.push_back(regCol);
		pauseTextWidth.push_back(menuText.totalW);
	}

	std::vector<glm::vec3> controllerColors = { glm::vec3(1), glm::vec3(0.0f, 0.7f, 0.2f), glm::vec3(0.0f, 0.2f, 0.7f), glm::vec3(0.7f, 0.1f, 0.2f), glm::vec3(0.7f, 0.7f, 0.2f) };
	// grey is 0, then the IDs
	
	
	std::vector<glm::vec3> playerColors = { glm::vec3(0.0f, 0.7f, 0.2f), glm::vec3(0.0f, 0.2f, 0.7f), glm::vec3(0.7f, 0.1f, 0.2f), glm::vec3(0.7f, 0.7f, 0.2f) };



	// Anti-Aliasing 
	unsigned int samples = 8;
	glfwWindowHint(GLFW_SAMPLES, samples);

	// Physx
	PhysicsManager pm = PhysicsManager(1.3f / 60.0f);
	PVehicle player = PVehicle(0, pm, VehicleType::eAVA_GREEN, PlayerOrAI::ePLAYER, PxVec3(0.0f, 25.f, 200.0f)); // p1 green car
	PVehicle enemy = PVehicle(1, pm, VehicleType::eAVA_BLUE, PlayerOrAI::eAI, PxVec3(0.0f, 25.f, -200.f)); // p2 blue car
	PVehicle enemy2 = PVehicle(2, pm, VehicleType::eAVA_RED, PlayerOrAI::eAI, PxVec3(200.0f, 25.0f, 0.0f)); // p3 red car
	PVehicle enemy3 = PVehicle(3, pm, VehicleType::eAVA_YELLOW, PlayerOrAI::eAI, PxVec3(-200.0f, 25.0f, 0.0f)); // p4 yellow car

	PowerUp powerUp1 = PowerUp(pm, Model("models/powerups/jump_star/star.obj"), PowerUpType::eJUMP, PxVec3(70.f, 20.f, 110.f));
	PowerUp powerUp2 = PowerUp(pm, Model("models/powerups/health_star/heart.obj"), PowerUpType::eHEALTH, PxVec3(115.f, 10.f, 20.f));
	PowerUp powerUp3 = PowerUp(pm, Model("models/powerups/health_star/heart.obj"), PowerUpType::eHEALTH, PxVec3(-120.f, 25.f, -111.f));
	PowerUp powerUp4 = PowerUp(pm, Model("models/powerups/jump_star/star.obj"), PowerUpType::eJUMP, PxVec3(77.f, 20.f, -113.f));
	PowerUp powerUp5 = PowerUp(pm, Model("models/powerups/shield/shieldman.obj"), PowerUpType::eSHIELD, PxVec3(0.f, 20.f, 0.f));
	PowerUp powerUp6 = PowerUp(pm, Model("models/powerups/shield/shieldman.obj"), PowerUpType::eSHIELD, PxVec3(-169.f, 32.f, 33.f));
	PowerUp powerUp7 = PowerUp(pm, Model("models/powerups/shield/shieldman.obj"), PowerUpType::eSHIELD, PxVec3(0.f, 90.f, 0.f));


	PStatic sphere = PStatic(pm, Model("models/sphere/sphere.obj"), PxVec3(0.f, 80.f, 0.f));

	std::vector<PVehicle*> vehicleList;
	std::vector<PowerUp*> powerUps;
	vehicleList.push_back(&player);
	vehicleList.push_back(&enemy);
	vehicleList.push_back(&enemy2);
	vehicleList.push_back(&enemy3);
	powerUps.push_back(&powerUp1);
	powerUps.push_back(&powerUp2);
	powerUps.push_back(&powerUp3);
	powerUps.push_back(&powerUp4);
	powerUps.push_back(&powerUp5);
	powerUps.push_back(&powerUp6);
	powerUps.push_back(&powerUp7);

	TextRenderer boost(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	boost.Load("freetype/fonts/vemanem.ttf", 100);

	// Create Grass
	std::vector<Model> grassPatches;
	std::vector<Model> trees;
	//renderer.generateLandscape(trees, grassPatches, pm.m_groundModel);

	// AI toggle
	bool ai_ON = true;
	bool singlePlayerIndicator = true;
	// Controller
	InputController controller1, controller2, controller3, controller4;

	// ImGui
	//ImguiManager imgui(window);




	// Menu
	GameManager::get().initMenu();

	std::string printDamage;
	std::string printNumbers;

	MiniMap map1(1, player);
	//GameManager::get().playerNumber = 2; // NUMBER OF VIEWPORTS

	Model bottom = Model("models/ground/iceberg.obj");
	Model bottom1 = Model("models/icebergs/blue_iceberg.obj");
	Model bottom2 = Model("models/icebergs/green_iceberg.obj");
	Model bottom3 = Model("models/icebergs/purple_iceberg.obj");
	Model bottom4 = Model("models/icebergs/red_iceberg.obj");

	Model toruses = Model("models/topofmap/toruses.obj");
	Model icosahedron = Model("models/topofmap/icosahedron.obj");

	Model spike1 = Model("models/topofmap/tealspike.obj");
	Model spike2 = Model("models/topofmap/redsmallspike.obj");
	Model spike3 = Model("models/topofmap/bigredspike.obj");
	Model spike4 = Model("models/topofmap/greyspike.obj");

	Texture white_heart("textures/white_heart.png", GL_LINEAR);

	float x = 0;
	float y = 0;

	float xgap = 0;
	float ygap = 0;

	time_point now = steady_clock::now();
	// Audio
	AudioManager::get().init(vehicleList);
	AudioManager::get().startCarSounds();
	AudioManager::get().setCarSoundsPause(true);

	std::vector<PVehicle*> winnerList = {&enemy};
	PVehicle* winnerCar = &enemy;


	while (!window.shouldClose() && !GameManager::get().quitGame) {

		// always update the time and poll events
		time.update();
		glfwPollEvents();
		glEnable(GL_DEPTH_TEST);

		if (time.shouldSimulate) {
			time.startSimTimer();
			AudioManager::get().update();
			AudioManager::get().updateBGM();
			// check controller connected; when we have more controllers we will make it into a loop
			// should probably put this away into the controller class
			if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
				if (!controller1.connected) {
					AudioManager::get().playSound(SFX_CONTROLLER_ON, 0.3f);
					controller1 = InputController(GLFW_JOYSTICK_1);
					controller1.connected = true;
					Log::debug("Controller 1 Connecter in main");

				}
			}
			else {
				if (controller1.connected) {
					AudioManager::get().playSound(SFX_CONTROLLER_OFF, 0.5f);
					controller1.connected = false;
					Log::debug("Controller 1 disconnected in main");

				}

			}

			if (glfwJoystickPresent(GLFW_JOYSTICK_2)) {
				if (!controller2.connected) {
					AudioManager::get().playSound(SFX_CONTROLLER_ON, 0.3f);
					controller2 = InputController(GLFW_JOYSTICK_2);

					controller2.connected = true;
					Log::debug("Controller 2 Connecter in main");
				}
			}
			else {
				if (controller2.connected) {
					AudioManager::get().playSound(SFX_CONTROLLER_OFF, 0.5f);
					controller2.connected = false;
					Log::debug("Controller 2 connected in main");

				}

			}

			if (glfwJoystickPresent(GLFW_JOYSTICK_3)) {
				if (!controller3.connected) {
					AudioManager::get().playSound(SFX_CONTROLLER_ON, 0.3f);
					controller3 = InputController(GLFW_JOYSTICK_3);

					controller3.connected = true;
					Log::debug("Controller 3 Connecter in main");
				}
			}
			else {
				if (controller3.connected) {
					AudioManager::get().playSound(SFX_CONTROLLER_OFF, 0.5f);
					controller3.connected = false;
					Log::debug("Controller 3 connected in main");

				}

			}
			if (glfwJoystickPresent(GLFW_JOYSTICK_4)) {
				if (!controller4.connected) {
					AudioManager::get().playSound(SFX_CONTROLLER_ON, 0.3f);
					controller4 = InputController(GLFW_JOYSTICK_4);

					controller4.connected = true;
					Log::debug("Controller 4 Connecter in main");
				}
			}
			else {
				if (controller4.connected) {
					AudioManager::get().playSound(SFX_CONTROLLER_OFF, 0.5f);
					controller4.connected = false;
					Log::debug("Controller 4 connected in main");

				}

			}

			switch (GameManager::get().screen) {
			case Screen::eMAINMENU: {
				if (controller1.connected) controller1.uniController(false, player);
				if (controller2.connected) controller2.uniController(false, enemy);
				if (controller3.connected) controller3.uniController(false, enemy2);
				if (controller4.connected) controller4.uniController(false, enemy3);

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
				for (int i = 0; i < GameManager::get().playerNumber; i++)
				{
					vehicleList[i]->setCar_tpye(PlayerOrAI::ePLAYER);

				}
				for (int i = 3; i >= GameManager::get().playerNumber; i--)
				{
					vehicleList[i]->setCar_tpye(PlayerOrAI::eAI);
				}
				AudioManager::get().setCarSoundsPause(false);
				AudioManager::get().startGame();
				GameManager::get().screen = Screen::ePLAYING;


				if (GameManager::get().playerNumber > 1 && !GameManager::get().multiplayer60FPS) time.toMultiplayerMode();
				else time.toSinglePlayerMode();

				break; }
			case Screen::ePLAYING: {
				AudioManager::get().setListenerPosition(Utils::instance().pxToGlmVec3(player.getPosition()), player.getFrontVec(), player.getUpVec());

				AudioManager::get().updateCarSounds();

				if (GameManager::get().paused) { // paused, read the inputs using the menu function
					if (controller1.connected) controller1.uniController(false, player);
					if (controller2.connected) controller2.uniController(false, enemy);
					if (controller3.connected) controller3.uniController(false, enemy2);
					if (controller4.connected) controller4.uniController(false, enemy3);
				}
				else { // in game

					if (controller1.connected) controller1.uniController(true, player);
					if (controller2.connected && enemy.m_carType == PlayerOrAI::ePLAYER && !singlePlayerIndicator) controller2.uniController(true, enemy);
					if (controller3.connected && enemy2.m_carType == PlayerOrAI::ePLAYER && !singlePlayerIndicator) controller3.uniController(true, enemy2);
					if (controller4.connected && enemy3.m_carType == PlayerOrAI::ePLAYER && !singlePlayerIndicator) controller4.uniController(true, enemy3);


					int deadCounter = 0;
					now = steady_clock::now();

					for (PVehicle* carPtr : vehicleList) {
						if (carPtr->vehicleAttr.collided) {
							carPtr->getRigidDynamic()->addForce((carPtr->vehicleAttr.forceToAdd), PxForceMode::eIMPULSE);
							carPtr->getRigidDynamic()->addForce(PxVec3(0.f, 10.f + 5.f * carPtr->vehicleAttr.collisionCoefficient, 0.f), PxForceMode::eVELOCITY_CHANGE);
							carPtr->flashWhite();
							carPtr->vehicleAttr.collided = false;
							AudioManager::get().playSound(SFX_CAR_HIT, Utils::instance().pxToGlmVec3(carPtr->vehicleAttr.collisionMidpoint), 0.3f);
						}

						if (carPtr->m_carType == PlayerOrAI::eAI && (carPtr->vehicleAttr.reachedTarget || duration_cast<seconds>(now - carPtr->vehicleAttr.targetTimestamp) > seconds(15)) ) {
							carPtr->vehicleAttr.targetTimestamp = now;
							int halfChance = Utils::instance().random(0, 2);
							if (halfChance == 0 || halfChance == 1) {
								int rndIndex = Utils::instance().random(0, (int)vehicleList.size() - 1);
								if (vehicleList[rndIndex] != carPtr && vehicleList[rndIndex]->m_state == VehicleState::ePLAYING) {
									carPtr->vehicleAttr.reachedTarget = false;
									carPtr->driveTo(vehicleList[rndIndex]->getPosition(), vehicleList[rndIndex], nullptr);
								}
							}
							else {
								int rndIndex = Utils::instance().random(0, (int)powerUps.size() - 1);
								if (powerUps[rndIndex]->active) {
									carPtr->vehicleAttr.reachedTarget = false;
									carPtr->driveTo(powerUps[rndIndex]->getPosition(), nullptr, powerUps[rndIndex]);
								}
								else {
									int rndIndex = Utils::instance().random(0, (int)vehicleList.size() - 1);
									if (vehicleList[rndIndex] != carPtr && vehicleList[rndIndex]->m_state == VehicleState::ePLAYING) {
										carPtr->vehicleAttr.reachedTarget = false;
										carPtr->driveTo(vehicleList[rndIndex]->getPosition(), vehicleList[rndIndex], nullptr);
									}
								}
							}
						}

						carPtr->updateState(); // to check for car death

						if (carPtr->m_state == VehicleState::eOUTOFLIVES) {
							deadCounter++;
							if (singlePlayerIndicator && player.m_state == VehicleState::eOUTOFLIVES) { // if single player died first
								AudioManager::get().gameOver();
								GameManager::get().screen = Screen::eGAMEOVER;
								GameManager::get().winner = 3; // set winner to 3 but not actually 3 because we are ending the game early
								winnerList.clear();
								winnerList.push_back(vehicleList.at(GameManager::get().winner));
								winnerCar = vehicleList.at(GameManager::get().winner);
								winnerCar->vehicleAttr.collisionCoefficient = 0.0f;
							}
						}

						cameraList.at(carPtr->carid)->m_fov = 80 + (carPtr->getRigidDynamic()->getLinearVelocity().magnitude() / 9.f);

					}

					if (deadCounter == (vehicleList.size() - 1)) {
						for (PVehicle* carPtr : vehicleList) {
							if (carPtr->m_state != VehicleState::eOUTOFLIVES) GameManager::get().winner = carPtr->carid;
						}
						AudioManager::get().gameOver();
						winnerList.clear();
						winnerList.push_back(vehicleList.at(GameManager::get().winner));
						winnerCar = vehicleList.at(GameManager::get().winner);
						winnerCar->vehicleAttr.collisionCoefficient = 0.0f;	
						//vehicleList.at(GameManager::get().winner)
						GameManager::get().screen = Screen::eGAMEOVER;
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
							if (vehicleList[i]->m_carType == PlayerOrAI::ePLAYER) continue;
							PVehicle* targetVehicle = (PVehicle*)vehicleList[i]->vehicleAttr.targetVehicle;
							PowerUp* targetPowerUp = (PowerUp*)vehicleList[i]->vehicleAttr.targetPowerup;
							if (targetVehicle) vehicleList[i]->driveTo(targetVehicle->getPosition(), targetVehicle, nullptr);
							else if (targetPowerUp) vehicleList[i]->driveTo(targetPowerUp->getPosition(), nullptr, targetPowerUp);
							else {
								int halfChance = Utils::instance().random(0, 2);
								if (halfChance == 0 || halfChance == 1) {
									int rndIndex = Utils::instance().random(0, (int)vehicleList.size() - 1);
									if (vehicleList[rndIndex] != vehicleList[i] && vehicleList[rndIndex]->m_state == VehicleState::ePLAYING) {
										vehicleList[i]->driveTo(vehicleList[rndIndex]->getPosition(), vehicleList[rndIndex], nullptr);
									}
								}
								else {
									int rndIndex = Utils::instance().random(0, (int)powerUps.size() - 1);
									if (powerUps[rndIndex]->active) vehicleList[i]->driveTo(powerUps[rndIndex]->getPosition(), nullptr, powerUps[rndIndex]);
									else {
										int rndIndex = Utils::instance().random(0, (int)vehicleList.size() - 1);
										if (vehicleList[rndIndex] != vehicleList[i] && vehicleList[rndIndex]->m_state == VehicleState::ePLAYING) {
											vehicleList[i]->driveTo(vehicleList[rndIndex]->getPosition(), vehicleList[rndIndex], nullptr);
										}
									}
								}
							}
						}
					}




					pm.simulate();

					for (PVehicle* vehicle : vehicleList) vehicle->updatePhysics();

					time.endSimTimer();
				}

				break; }
			case Screen::eGAMEOVER: {
				if (controller1.connected) controller1.uniController(false, player);
				if (controller2.connected) controller2.uniController(false, enemy);
				if (controller3.connected) controller3.uniController(false, enemy2);
				if (controller4.connected) controller4.uniController(false, enemy3);
				break; }
			}
			time.endSimTimer(); // end sim timer !
		}

		if (time.shouldRender) {
			time.startRenderTimer();
			renderer.startFrame();
			switch (GameManager::get().screen) {
			case Screen::eMAINMENU: {
				singlePlayerIndicator = true;
				renderer.m_currentViewportActive = 4;
				renderer.skybox.draw(menuCamera.getPerspMat(), glm::mat4(glm::mat3(menuCamera.getViewMat())));

				os = (sin((float)colorVar / 20) + 1.0) / 2.0;
				colorVar++;
				renderer.renderNormalObjects(trees, grassPatches); // prepare to draw NORMAL objects, doesn't actually render anything.
				pm.drawGround();

				renderer.renderTransparentObjects(vehicleList, sphere, os, time);

				bottom.draw();
				bottom1.draw();
				bottom2.draw();
				bottom3.draw();
				bottom4.draw();
				toruses.draw();

				spike1.draw();
				spike2.draw();
				spike3.draw();
				spike4.draw();


				switch (GameManager::get().mainMenuScreen) {
				case MainMenuScreen::eMAIN_SCREEN: {

					for (int i = 0; i < 6; i++) {
						if ((int)GameManager::get().menuButton == i) buttonColors.at(i) = selCol;
						else buttonColors.at(i) = regCol;
					}
					menuText.RenderText("SINGLEPLAYER", 50, 283, 1.2f, buttonColors.at(0));
					menuTextWidth.at(0) = menuText.totalW;
					menuText.RenderText("MULTIPLAYER", 50, 283 + 114, 1.2f, buttonColors.at(1));
					menuTextWidth.at(1) = menuText.totalW;
					menuText.RenderText("HOW TO PLAY", 50, 283 + 114 * 2, 1.2f, buttonColors.at(2));
					menuTextWidth.at(2) = menuText.totalW;
					menuText.RenderText("OPTIONS", 50, 283 + 114 * 3, 1.2f, buttonColors.at(3));
					menuTextWidth.at(3) = menuText.totalW;
					menuText.RenderText("CREDITS", 50, 283 + 114 * 4, 1.2f, buttonColors.at(4));
					menuTextWidth.at(4) = menuText.totalW;
					menuText.RenderText("QUIT", 50, 283 + 114 * 5, 1.2f, buttonColors.at(5));
					menuTextWidth.at(5) = menuText.totalW;
					mainMenu.draw(menu, glm::vec2(944, 635), glm::vec2(1.492f * 557.f, 1.492 * 284.f), 0, glm::vec3(1.f, 1.f, 1.f));

					

					break; }
				case MainMenuScreen::eMULTIPLAYER_SCREEN:
					singlePlayerIndicator = false;
					for (int i = 0; i < 2; i++) {
						if ((int)GameManager::get().playerSelectButton == i) playerSelectButtonColors.at(i) = selCol;
						else playerSelectButtonColors.at(i) = regCol;
					}

					menuText.RenderText("Select number of players: " + std::to_string(GameManager::get().playerNumber), 94.f, 447.f, 1.0f, playerSelectButtonColors.at(0));
					menuText.RenderText("START", 94.f, 547.f, 1.0f, playerSelectButtonColors.at(1));
					menuText.RenderText("Hold START to check controller", 1034.f, 50.f, 1.0f, regCol);



					if (controller1.connected) image1.draw(con, glm::vec2(1047.f, 598.f), glm::vec2(320.f, 160.f), 0, controllerColors.at(controller1.startHeld * 1));
					if (controller2.connected) image1.draw(con, glm::vec2(1047.f + 440.f, 598.f), glm::vec2(320.f, 160.f), 0, controllerColors.at(controller2.startHeld * 2));
					if (controller3.connected) image1.draw(con, glm::vec2(1047.f, 598.f + 250.f), glm::vec2(320.f, 160.f), 0, controllerColors.at(controller3.startHeld * 3));
					if (controller4.connected) image1.draw(con, glm::vec2(1047.f + 440.f, 598.f + 250.f), glm::vec2(320.f, 160.f), 0, controllerColors.at(controller4.startHeld * 4));

					break;
				case MainMenuScreen::eHOWTOPLAY_SCREEN:
					image1.draw(texture, glm::vec2(0.f, 0.f), glm::vec2(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT), 0, glm::vec3(1.f, 1.f, 1.f));


					break;
				case MainMenuScreen::eOPTIONS_SCREEN:
					for (int i = 0; i < 4; i++) {
						if ((int)GameManager::get().optionsButton == i) optionsButtonColors.at(i) = selCol;
						else optionsButtonColors.at(i) = regCol;
					}

					menuText.RenderText("BGM: " + std::to_string(AudioManager::get().getBGMLevel()), 165, 310, 1.0f, optionsButtonColors.at(0));
					menuText.RenderText("SFX: " + std::to_string(AudioManager::get().getSFXLevel()), 165, 310 + 105, 1.0f, optionsButtonColors.at(1));
					menuText.RenderText("Multiplayer FPS:  " + GameManager::get().getMultiplayerFPS() ,165, 310 + 105 * 2, 1.0f, optionsButtonColors.at(2));
					menuText.RenderText("BACK", 165, 310 + 105 * 3, 1.0f, optionsButtonColors.at(3));


					break;
				case MainMenuScreen::eCREDITS_SCREEN:
					menuText.RenderText("Andre Staffa", Utils::instance().SCREEN_WIDTH / 3, 400.f, 1.0f, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));
					menuText.RenderText("Taras Leshchenko", Utils::instance().SCREEN_WIDTH / 3, 500.f, 1.0f, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));
					menuText.RenderText("Huanjun Zhao", Utils::instance().SCREEN_WIDTH / 3, 600.f, 1.0f, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));
					menuText.RenderText("Callaghan Davitt", Utils::instance().SCREEN_WIDTH / 3, 700.f, 1.0f, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));
					menuText.RenderText("Evan Wong", Utils::instance().SCREEN_WIDTH / 3, 800.f, 1.0f, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));
					menuText.RenderText("Wacky Rotation Studios 2022. All Rights Reserved.", Utils::instance().SCREEN_WIDTH / 3, 1000.f, 1.0f, glm::vec3(1.f, 170.f, 5.f));
					menuText.RenderText("V1.0", 15.f, Utils::instance().SCREEN_HEIGHT - 50.f, 1, glm::vec3(0.f));
					break;
				}
				
				break; }
			case Screen::eLOADING: {
				renderer.skybox.draw(menuCamera.getPerspMat(), glm::mat4(glm::mat3(menuCamera.getViewMat())));

				// imGUI section
				/*imgui.initFrame();
				imgui.renderMenu(ai_ON);
				imgui.endFrame();*/

				break; }

			case Screen::ePLAYING: {
				printNumbers = "      ";
				for (PVehicle* carPtr : vehicleList) {
					printNumbers += fmt::format("{:.1f}", carPtr->vehicleAttr.collisionCoefficient);
					printNumbers += " ";
				}
				printNumbers += "         ";
				for (PVehicle* carPtr : vehicleList) {
					printNumbers += std::to_string(carPtr->m_lives);
					printNumbers += "    ";
				}


				for (int currentViewport = 0; currentViewport < GameManager::get().playerNumber; currentViewport++) {
					renderer.switchViewport(GameManager::get().playerNumber, currentViewport);
					cameraList.at(currentViewport)->updateCameraPosition(Utils::instance().pxToGlmVec3(vehicleList.at(currentViewport)->getPosition()), vehicleList.at(currentViewport)->getFrontVec()); // only move cam once.
					//map1.displayMap(player, &vehicleList, &imageList, currentViewport);

					os = (sin((float)colorVar / 20) + 1.0) / 2.0;
					colorVar++;
					renderer.renderShadows(vehicleList, powerUps);
					renderer.skybox.draw(cameraList.at(currentViewport)->getPerspMat(), glm::mat4(glm::mat3(cameraList.at(currentViewport)->getViewMat())));
					renderer.renderCars(vehicleList);
					renderer.renderPowerUps(powerUps, os);
					renderer.renderNormalObjects(trees, grassPatches); // prepare to draw NORMAL objects, doesn't actually render anything.
					pm.drawGround();

					renderer.renderTransparentObjects(vehicleList, sphere, os, time);

					bottom.draw();
					bottom1.draw();
					bottom2.draw();
					bottom3.draw();
					bottom4.draw();
					toruses.draw();

					spike1.draw();
					spike2.draw();
					spike3.draw();
					spike4.draw();


					renderer.useDefaultShader();
					map1.displayMap(player, &vehicleList, &imageList, currentViewport);


					if (GameManager::get().paused) {
						for (int i = 0; i < 2; i++) {
							if ((int)GameManager::get().pauseButton == i) pausedButtonColors.at(i) = selCol;
							else pausedButtonColors.at(i) = regCol;
						}
						menuText.RenderText("PAUSED",30 ,44 -20, 2,  glm::vec3(0.992f, 0.164f, 0.129f));
						menuText.RenderText("RESUME",30, 154 - 20, 1.5f, pausedButtonColors.at(0));
						pauseTextWidth.at(0) = menuText.totalW;
						menuText.RenderText("QUIT", 30, 154 + 91 - 20, 1.5f, pausedButtonColors.at(1));
						pauseTextWidth.at(1) = menuText.totalW;
					}

					//menuText.RenderText("Damage: P1  P2  P3  P4 Lives: P1  P2  P3  P4", 7.547f, 7.547f, 0.5f, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));
					//menuText.RenderText(printNumbers, 60.f, 30.f, 0.5f, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));

					for (PVehicle* carPtr : vehicleList) {
						for (int i = 0; i < carPtr->m_lives; i++) {
							image1.draw(white_heart, glm::vec2(635.f + (carPtr->carid * 180.f) + (i * 38), 20 + 72 - 14), glm::vec2(30, 30), 0, playerColors.at(carPtr->carid)); //x = 160 OG
							//image1.draw(white_heart, glm::vec2(x + (carPtr->carid * xgap) + (i * ygap), y), glm::vec2(30, 30), 0, playerColors.at(carPtr->carid)); // x 15 y 141 xgap 163 ygap 38
						}
						//fmt::format("{:.1f}", carPtr->vehicleAttr.collisionCoefficient);
						menuText.RenderText(fmt::format("{:.1f}", carPtr->vehicleAttr.collisionCoefficient * 19.f) + "%", 635.f + (carPtr->carid * 180.f), 20, 1.131, glm::vec3(0.f, 0.f, 0.f));
						//menuText.RenderText(fmt::format("{:.1f}", carPtr->vehicleAttr.collisionCoefficient) + "%", 15 + (carPtr->carid * x), 400.f, 1.131, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));
					}

					//menuText.RenderText(printNumbers, x, y, xgap, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));

					boost.RenderText(std::to_string(vehicleList.at(currentViewport)->vehicleParams.boost), 15.f, Utils::instance().SCREEN_HEIGHT - 85.0f, 1.0f, glm::vec3(0.992f, 0.164f, 0.129f));
					switch (vehicleList.at(currentViewport)->getPocket()) {
					case PowerUpType::eEMPTY:
						//currentPowerup.RenderText("Pocket: Empty", 7.547f, 60.f, 1.0f, glm::vec3(0.478f, 0.003f, 0.f));
						break;
					case PowerUpType::eJUMP:
						//currentPowerup.RenderText("Pocket: Jump", 7.547f, 60.f, 1.0f, glm::vec3(1.f, 0.050f, 0.039f));
						image1.draw(star, glm::vec2(Utils::instance().SCREEN_WIDTH - 250.f, Utils::instance().SCREEN_HEIGHT - 250.f), glm::vec2(250.f, 250.f), 0, glm::vec3(1.f, 1.f, 0));
						break;
					case PowerUpType::eSHIELD:
						//currentPowerup.RenderText("Pocket: Shield", 7.547f, 60.f, 1.0f, glm::vec3(1.f, 0.050f, 0.039f));
						image1.draw(shield, glm::vec2(Utils::instance().SCREEN_WIDTH - 250.f, Utils::instance().SCREEN_HEIGHT - 250.f), glm::vec2(250.f, 250.f), 0, glm::vec3(1.0f, 0.5f, 0.31f));
						break;

					}


				}

				break; }
			case Screen::eGAMEOVER: {	

				renderer.m_currentViewportActive = 4;
				renderer.skybox.draw(menuCamera.getPerspMat(), glm::mat4(glm::mat3(menuCamera.getViewMat())));

				os = (sin((float)colorVar / 20) + 1.0) / 2.0;
				colorVar++;
				renderer.renderNormalObjects(trees, grassPatches); // prepare to draw NORMAL objects, doesn't actually render anything.
				pm.drawGround();

				if (winnerList.size() > 0)
				{
					winnerCar->getRigidDynamic()->setGlobalPose(PxTransform(PxVec3(-242.f, 300.f, 380.f), PxQuat(PxPi, PxVec3(0.f, 0.f, 0.f))));
					renderer.renderCars(winnerList);
				}

		
				renderer.renderTransparentObjects(vehicleList, sphere, os, time);

				bottom3.draw();
				toruses.draw();

				spike3.draw();
				spike4.draw();

				menuText.RenderText("Game Over", 123, 323,1,  glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));
				menuText.RenderText("Player " + std::to_string(GameManager::get().winner + 1) + " wins",123, 323 + 120, 1.0f, glm::vec3(204.f / 255.f, 0.f, 102.f / 255.f));
				menuText.RenderText("QUIT ", 123, 323 + 120 * 2, 1.0f, selCol);


				break; }
			}

			renderer.endFrame();
			glViewport(0, 0, Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT); // bring the viewport back to original
			time.endRenderTimer();



		}

	}

	player.free();
	enemy.free();
	pm.free();
	//imgui.freeImgui();

	glfwTerminate();
	return 0;
}
