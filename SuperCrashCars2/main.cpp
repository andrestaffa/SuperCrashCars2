#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "Time.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

#include "PVehicle.h"
#include "PDynamic.h"
#include "PStatic.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(std::vector<std::string> faces);

int main(int argc, char** argv) {
	Log::info("Starting Game...");

#pragma region setup

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
	playerCamera.setPitch(-30.0f);

	// Physx
	PhysicsManager pm = PhysicsManager(1.5f/60.0f);
	PVehicle player = PVehicle(pm, VehicleType::eJEEP, PxVec3(0.0f, 10.0f, 0.0f));
	PVehicle enemy = PVehicle(pm, VehicleType::eTOYOTA, PxVec3(5.0f, 10.0f, 0.0f));
	player.vehicleParams.jumpCoefficient = player.getRigidDynamic()->getMass() * 7;
	player.vehicleParams.boostCoefficient = player.getRigidDynamic()->getMass() / 3;
	
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

	// Controller
	InputController controller;
	if (glfwJoystickPresent(GLFW_JOYSTICK_1)) controller = InputController(GLFW_JOYSTICK_1);

	// Anti-Aliasing not sure if this works rn becuase doesn't work for frame buffer, but we are missing some parts of frame buffer if we use it can't tell
	unsigned int samples = 8;
	glfwWindowHint(GLFW_SAMPLES, samples);

#pragma endregion

#pragma region skybox

	auto skyboxShader = std::make_shared<ShaderProgram>("shaders/skybox.vert", "shaders/skybox.frag");
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	std::vector<std::string> faces
	{
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/front.jpg",
		"skybox/back.jpg",
	};
	unsigned int cubemapTexture = loadCubemap(faces);



	Utils::instance().shader = skyboxShader;
	Utils::instance().shader->use();
	glUniform1i(glGetUniformLocation(*Utils::instance().shader, "skybox"), 0);

	/*skyboxShader.use();
	skyboxShader.setInt("skybox", 0);*/

#pragma endregion 

	while (!window.shouldClose()) {

		glfwPollEvents();

		Time::update();

		// if 8333 microseconds passed, simulate
		if (Time::shouldSimulate) {
			Time::startSimTimer();
			//Log::info("Starting Simulation...");
#pragma region controller_inputs

			if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
				controller.PS4Input(player);
				//controller.XboxInput(player);
			}

#pragma endregion

#pragma region keyboard_inputs

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
			if (inputManager->onKeyAction(GLFW_KEY_R, GLFW_PRESS)) player.getRigidDynamic()->setGlobalPose(PxTransform(PxVec3(0.0f, 10.0f, 0.0f), PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f))));

#pragma endregion

			pm.simulate();
			player.update();
			enemy.update();

			Time::simulatePhysics();  // turn off the simulation flag and stop timer
		}
		//if 16666 microseconds passed, render
		if (Time::shouldRender) {
			Time::startRenderTimer();
			glfwPollEvents();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();



			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE); // if faces are randomly missing try
			glCullFace(GL_FRONT);	// commenting out these three lines
			glFrontFace(GL_CW);		// 
			glEnable(GL_MULTISAMPLE);
			glEnable(GL_LINE_SMOOTH);
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Utils::instance().shader = defualt;
			Utils::instance().shader->use();
			glUniform4f(glGetUniformLocation(*Utils::instance().shader, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
			glUniform3f(glGetUniformLocation(*Utils::instance().shader, "lightPos"), player.getPosition().x, player.getPosition().y, player.getPosition().z);
			glUniform3f(glGetUniformLocation(*Utils::instance().shader, "camPos"), playerCamera.getPosition().x, playerCamera.getPosition().y, playerCamera.getPosition().z);


			// update the camera based on front vec and player car position

			PxVec3 pxPlayerPos = player.getPosition();
			glm::vec3 glmPlayerPos = glm::vec3(pxPlayerPos.x, pxPlayerPos.y, pxPlayerPos.z);
			playerCamera.updateCamera(glmPlayerPos, player.getFrontVec());
			playerCamera.UpdateMVP();
			playerCamera.updateShaderUniforms();

			pm.drawGround();
			enemy.render();
			//skybox.draw();
			player.render();



			// draw skybox as last
			glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
			Utils::instance().shader = skyboxShader;
			Utils::instance().shader->use();
			glm::mat4 view = glm::mat4(glm::mat3(playerCamera.getViewMat())); // remove translation from the view matrix
			glm::mat4 projection = playerCamera.getPerspMat();
			glUniformMatrix4fv(glGetUniformLocation(*Utils::instance().shader, "view"), 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(*Utils::instance().shader, "projection"), 1, GL_FALSE, &projection[0][0]);

			// skybox cube
			glBindVertexArray(skyboxVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS); // set depth function back to default



			// imGUI section
			ImGui::Begin("Stats:");
			std::string simTime = ("Average Simulation time: " + std::to_string(Time::averageSimTime) + " microseconds");
			std::string renderTime = ("Average Render Time: " + std::to_string(Time::averageRenderTime) + " microseconds");
			std::string printBoost = ("Boost: " + std::to_string(player.vehicleParams.boost));
			std::string printPos = "Current Position: X: " + std::to_string(player.getPosition().x) + " Y: " + std::to_string(player.getPosition().y) + " Z: " + std::to_string(player.getPosition().z);
			std::string printLinearVelocity = "Current Linear Velocity: X: " + std::to_string(player.getRigidDynamic()->getLinearVelocity().x) + " Y: " + std::to_string(player.getRigidDynamic()->getLinearVelocity().y) + " Z: " + std::to_string(player.getRigidDynamic()->getLinearVelocity().z);
			std::string printAngularVelocity = "Current Angular Velocity: X: " + std::to_string(player.getRigidDynamic()->getAngularVelocity().x) + " Y: " + std::to_string(player.getRigidDynamic()->getAngularVelocity().y) + " Z: " + std::to_string(player.getRigidDynamic()->getAngularVelocity().z);

			ImGui::Text(simTime.c_str());
			ImGui::Text(renderTime.c_str());
			ImGui::Text(printBoost.c_str());
			ImGui::Text(printPos.c_str());
			ImGui::Text(printLinearVelocity.c_str());
			ImGui::Text(printAngularVelocity.c_str());
			/*
			ImGui::Text("");
			ImGui::Text("Controls:");
			ImGui::Text("Controller: Use left stick for steering while on the ground, and to turn in the air");
			ImGui::Text("Use R / L or Up/Down for to speed up and slow down.");
			ImGui::Text("E(Kbord)/X(PS4)/A(XBOX) - jump");
			ImGui::Text("F(kbord)/TRIANGLE(PS4)/Y(XBOX) = jet boost");
			ImGui::Text("R(keyboard)/SHARE(PS4)/BACK(XBOX) - Reset");
			ImGui::Text("Spacebar(keyboard)/SQUARE(PS4)/X(XBOX) - handbrake");
			ImGui::Text("Keyboard:Use Right/Left arrow keys to turn.");
			*/

			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


			glDisable(GL_FRAMEBUFFER_SRGB);
			window.swapBuffers();
			Time::renderFrame(); // turn off the render flag and stop timer
		}
	}

	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVAO);

	player.free();
	enemy.free();
	pm.free();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	glfwTerminate();
	return 0;
}



// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format{};
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}