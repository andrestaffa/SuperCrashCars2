#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "Window.h"
#include <cmath>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/type_ptr.hpp"

#include "Camera.h"
#include "Skybox.h"
#include "TextRenderer.h"

#include "PVehicle.h"
#include "PDynamic.h"
#include "PStatic.h"
#include "PowerUp.h"

#include "Time.h"

class RenderManager {

public:
	RenderManager(Window* window, std::vector<Camera*> *cameraList, Camera* menuCamera);

	Window* m_window;
	//Camera* m_camera;
	Camera* m_menuCamera;
	std::vector<Camera*> *m_cameraList;

	int m_currentViewportActive; // for camera selection

	std::shared_ptr<ShaderProgram> defaultShader, depthShader, carShader, transparentShader, powerUpShader;

	Skybox skybox;

	const glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	const glm::vec3 lightPos = glm::vec3(300.0f, 400.0f, 0.0f);

#pragma region shadow_init
	// Shadows
	const unsigned int SHADOW_WIDTH = 2048 * 4, SHADOW_HEIGHT = 2048 * 4;
	unsigned int depthMapFBO;

	// create depth texture
	unsigned int depthMap;

	float borderColor[4] = { 1.0, 1.0, 1.0, 1.0 };


	glm::mat4 lightView, lightSpaceMatrix;
	const glm::mat4 lightProjection = glm::ortho(-300.0f, 300.0f, -300.0f, 300.0f, 0.1f, 1000.0f);
	const float near_plane = 1.0f, far_plane = 7.5f;

#pragma endregion

	void startFrame();
	void endFrame();
	bool switchViewport(int playerNumber, int i);
	void renderShadows(const std::vector<PVehicle*>& vehicleList, const std::vector<PowerUp*>& powerUps);

	void renderCars(const std::vector<PVehicle*>& vehicleList);

	void renderNormalObjects(std::vector<Model>& trees, std::vector<Model>& grassPatches);
	void generateLandscape(std::vector<Model>& trees, std::vector<Model>& grassPatches, Model& ground);

	void renderTransparentObjects(const std::vector<PVehicle*>& vehicleList, PStatic& sphere, double os, Time& time);

	void renderPowerUps(const std::vector<PowerUp*>& powerUps, double os);

	void useDefaultShader();

private:

};