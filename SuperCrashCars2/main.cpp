#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Time.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "Window.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "InputManager.h"
#include "Camera.h"

#include "PVehicle.h"
#include "PDynamic.h"
#include "PStatic.h"


int main(int argc, char** argv) {
	Log::info("Starting Game...");

	// OpenGL
	glfwInit();
	Window window(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT, "Super Crash Cars 2");
	
	auto defualt = std::make_shared<ShaderProgram>("shaders/shader_vertex.vert", "shaders/shader_fragment.frag");
	auto light = std::make_shared<ShaderProgram>("shaders/light.vert", "shaders/light.frag");

	std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	window.setCallbacks(inputManager);

	// Camera
	bool cameraToggle = false;
	Camera playerCamera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	Camera editorCamera = Camera(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT, glm::vec3(-2.0f, 4.0f, 10.0f));
	playerCamera.setPitch(-30.0f);

	// Physx
	double playerMass;
	VehicleType playerType = VehicleType::eJEEP;

	if (playerType == VehicleType::eTOYOTA) playerMass = 8000.0;
	if (playerType == VehicleType::eJEEP) playerMass = 1500.0;
	if (playerType == VehicleType::eSHUCKLE) playerMass = 1000.0;
	double jumpCoefficient = playerMass * 7;
	VehicleType enemyType = VehicleType::eTOYOTA;

	float throttle = 1.0f;
	PhysicsManager pm = PhysicsManager(1.0f/60.0f);
	PVehicle player = PVehicle(pm, playerType, PxVec3(0.0f, 10.0f, 0.0f));
	PVehicle enemy = PVehicle(pm, enemyType, PxVec3(5.0f, 10.0f, 0.0f));

	Model skybox = Model("models/anime/skybox.obj");
	skybox.scale(glm::vec3(30, 30, 30));


	// ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Lighting
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Anti-Aliasing not sure if this works rn becuase doesn't work for frame buffer, but we are missing some parts of frame buffer if we use it can't tell
	unsigned int samples = 8;
	glfwWindowHint(GLFW_SAMPLES, samples);


	while (!window.shouldClose()) {

		Time::update();

		pm.simulate();
		player.update();
		enemy.update();
		glfwPollEvents();

		Utils::instance().shader = defualt;
		glUniform4f(glGetUniformLocation(*Utils::instance().shader, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(*Utils::instance().shader, "lightPos"), player.getPosition().x, player.getPosition().y, player.getPosition().z);
		Utils::instance().shader->use();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		#pragma region inputs

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

		if (inputManager->onKeyAction(GLFW_KEY_UP, GLFW_PRESS)) player.accelerate(throttle);
		if (inputManager->onKeyAction(GLFW_KEY_DOWN, GLFW_PRESS)) player.reverse(throttle * 0.5f);
		if (inputManager->onKeyAction(GLFW_KEY_LEFT, GLFW_PRESS)) player.turnLeft(throttle * 0.5f);
		if (inputManager->onKeyAction(GLFW_KEY_RIGHT, GLFW_PRESS)) player.turnRight(throttle * 0.5f);
		if (inputManager->onKeyAction(GLFW_KEY_SPACE, GLFW_PRESS)) player.handbrake();

		if (inputManager->onKeyAction(GLFW_KEY_E, GLFW_PRESS) && Time::interval(2.0f)) 
			player.getRigidDynamic()->addForce(PxVec3(0.0, 4500 + jumpCoefficient, 0.0), PxForceMode::eIMPULSE);

		if (inputManager->onKeyAction(GLFW_KEY_C, GLFW_PRESS) && Time::interval(1.0f))
			cameraToggle = !cameraToggle;

		#pragma endregion

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE); // if faces are randomly missing try
		glCullFace(GL_FRONT);	// commenting out these three lines
		glFrontFace(GL_CW);		// 
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_LINE_SMOOTH);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (cameraToggle)
			editorCamera.render();
		else {
			// update the camera based on front vec and player car position
			PxVec3 pxPlayerPos = player.getPosition();
			glm::vec3 glmPlayerPos = glm::vec3(pxPlayerPos.x, pxPlayerPos.y, pxPlayerPos.z);
			playerCamera.updateCamera(glmPlayerPos, player.getFrontVec());
			//playerCamera.setPosition(glm::vec3(player.getPosition().x, player.getPosition().y + 6.0f, player.getPosition().z + 12.0f));
			playerCamera.render();
		}

		pm.drawGround();
		enemy.render();
		skybox.draw();

		Utils::instance().shader = defualt;
		glUniform4f(glGetUniformLocation(*Utils::instance().shader, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(*Utils::instance().shader, "lightPos"), player.getPosition().x, player.getPosition().y, player.getPosition().z);
		glUniform3f(glGetUniformLocation(*Utils::instance().shader, "camPos"), playerCamera.getPosition().x, playerCamera.getPosition().y, playerCamera.getPosition().z);
		Utils::instance().shader->use();

		player.render();


		ImGui::Begin("Information/Controls");
		std::string fps = ("FPS " + std::to_string((int)Time::fps));
		ImGui::Text(fps.c_str());
		ImGui::Text("");
		ImGui::Text("Drive with arrow keys");
		ImGui::Text("E = jump");
		ImGui::Text("Spacebar = handbrake");
		ImGui::Text("C = toggle between editor and player cam");
		ImGui::Text("wasd + right-click/hold mouse = control editor cam");
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glDisable(GL_FRAMEBUFFER_SRGB);
		window.swapBuffers();
		
	}

	player.free();
	enemy.free();
	pm.free();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	glfwTerminate();
	return 0;
}
