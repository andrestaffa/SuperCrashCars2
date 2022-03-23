#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

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
	void renderStats(const PVehicle& player, int avgSimTime, int avgRenderTime);
	void renderSliders(const PVehicle& player, const PVehicle& enemy);
	void renderMenu(bool &AIToggle);
	void renderPlayerHUD(const PVehicle& player);
	void renderDamageHUD(const std::vector<PVehicle*>& carList);

	void freeImgui();

};