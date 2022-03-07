#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "Time.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Window.h"

#include "PVehicle.h"

#include "GameManager.h"



class ImguiManager {

public:
	ImguiManager(Window &window);

	void initFrame();
	void endFrame();
	void renderStats(const PVehicle& player);
	void renderSliders(const PVehicle& player, const PVehicle& enemy);
	void renderMenu();
	void renderPlayerHUD(const PVehicle& player);
	void renderDamageHUD(const std::vector<PVehicle*>& carList);

	void freeImgui();

};