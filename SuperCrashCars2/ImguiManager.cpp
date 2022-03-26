#include "ImguiManager.h"
#include "fmt/core.h"

ImguiManager::ImguiManager(Window& window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
};

void ImguiManager::initFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
};
void ImguiManager::endFrame() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
};
void ImguiManager::renderStats(const PVehicle& player, int avgSimTime, int avgRenderTime) {
	ImGui::Begin("Stats:");
	std::string simTime = ("Average Simulation time: " + std::to_string(avgSimTime) + " microseconds");
	std::string renderTime = ("Average Render Time: " + std::to_string(avgRenderTime) + " microseconds");
	std::string printPos = "Current Position: X: " + std::to_string(player.getPosition().x) + " Y: " + std::to_string(player.getPosition().y) + " Z: " + std::to_string(player.getPosition().z);
	std::string printLinearVelocity = "Current Linear Velocity: X: " + std::to_string(player.getRigidDynamic()->getLinearVelocity().x) + " Y: " + std::to_string(player.getRigidDynamic()->getLinearVelocity().y) + " Z: " + std::to_string(player.getRigidDynamic()->getLinearVelocity().z);
	std::string printAngularVelocity = "Current Angular Velocity: X: " + std::to_string(player.getRigidDynamic()->getAngularVelocity().x) + " Y: " + std::to_string(player.getRigidDynamic()->getAngularVelocity().y) + " Z: " + std::to_string(player.getRigidDynamic()->getAngularVelocity().z);
	//std::string printScreen = ("Screen is : " + std::to_string("Men")); Menu::GameState::

	ImGui::Text(simTime.c_str());
	ImGui::Text(renderTime.c_str());
	ImGui::Text(printPos.c_str());
	ImGui::Text(printLinearVelocity.c_str());
	ImGui::Text(printAngularVelocity.c_str());

	ImGui::End();
};
void ImguiManager::renderSliders(const PVehicle& player, const PVehicle& enemy) {

	ImGui::Begin("Sliders:");

	// slider for player mass
	float pmass = player.getRigidDynamic()->getMass();
	ImGui::SliderFloat("Player Mass", &pmass, 100.0f, 10000.0f);
	player.getRigidDynamic()->setMass(pmass);

	// slider for enemy mass
	float emass = enemy.getRigidDynamic()->getMass();
	ImGui::SliderFloat("Enemy  Mass", &emass, 100.0f, 10000.0f);
	enemy.getRigidDynamic()->setMass(emass);

	ImGui::End();
};
void ImguiManager::renderMenu(bool &AItoggle) {
	ImGui::Begin("MENU:");
	std::string menuDisplay = GameManager::get().printMenu();

	ImGui::Text(menuDisplay.c_str());
	bool BGMtoggle = AudioManager::get().getBGMMute();
	ImGui::Checkbox("Mute BGM", &BGMtoggle);
	if (BGMtoggle != AudioManager::get().getBGMMute()) AudioManager::get().toggleBGMMute();

	bool SFXtoggle = AudioManager::get().getSFXMute();
	ImGui::Checkbox("Mute SFX", &SFXtoggle);
	if (SFXtoggle != AudioManager::get().getSFXMute()) AudioManager::get().toggleSFXMute();

	ImGui::Checkbox("AI Toggle", &AItoggle);

	ImGui::End();
}
void ImguiManager::renderPlayerHUD(const PVehicle& player){
	ImGui::Begin("Player HUD:");

	std::string printBoost = ("Boost: " + std::to_string(player.vehicleParams.boost));
	std::string printPocket = ("Pocket: ");

	switch (player.getPocket()) {
	case PowerUpType::eEMPTY:
		printPocket += std::string("Empty");
		break;
	case PowerUpType::eJUMP:
		printPocket += std::string("Jump");
		break;

	case PowerUpType::eSHIELD:
		printPocket += std::string("Shield");
		break;

	}

	ImGui::Text(printBoost.c_str());
	ImGui::Text(printPocket.c_str());

	ImGui::End();
};
void ImguiManager::renderDamageHUD(const std::vector<PVehicle*>& carList) {
	ImGui::Begin("damage/lives HUD:");

	std::string printDamage = ("Damage: P1, P2 Lives: P1, P2\n        ");
	for (PVehicle* carPtr : carList) {
		printDamage += fmt::format("{:.1f}", carPtr->vehicleAttr.collisionCoefficient);
		printDamage += " ";
	}
	printDamage += "      ";
	for (PVehicle* carPtr : carList) {
		printDamage += std::to_string(carPtr->m_lives);
		printDamage += "   ";
	}

	ImGui::Text(printDamage.c_str());
	ImGui::End();
};

void ImguiManager::freeImgui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
};

