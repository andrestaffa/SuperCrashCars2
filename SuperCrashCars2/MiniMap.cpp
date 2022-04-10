#include "MiniMap.h"

MiniMap::MiniMap() {}

MiniMap::MiniMap(int playerId, PVehicle& player) {
	playerId = this->playerId;
	frontVec = player.getFrontVec();
	currentPos = player.getPosition();
}



void MiniMap::updateMap(PVehicle& player, const std::vector<PVehicle*>& vehicleList)
{
	frontVec = player.getFrontVec();
	currentPos = player.getPosition();
	PxVec3 relativePosition;
	for (auto carPtr : vehicleList) {

		oppoPos.push_back(carPtr->getFrontVec());
		oppoFrontVec.push_back(carPtr->getPosition());
		relativePosition = (player.getPosition() - carPtr->getPosition());
	}

}


void MiniMap::displayMap(PVehicle& player, const std::vector<PVehicle*>* vehicleList, std::vector<Image*> *imageList, int currentPlayer) {
	Texture green("textures/green.png", GL_LINEAR); // Just a placeholder/test image
	Texture blue("textures/blue.png", GL_LINEAR); // Just a placeholder/test image
	Texture red("textures/red.png", GL_LINEAR); // Just a placeholder/test image
	Texture yellow("textures/yellow.png", GL_LINEAR); // Just a placeholder/test image
	//Single player
	for (size_t i = 0; i < 4; i++)
	{
		float startPosX = Utils::instance().SCREEN_WIDTH - 110.f;
		float startPosY = Utils::instance().SCREEN_HEIGHT - 630.f;
		float mapposX = Utils::instance().pxToGlmVec3(vehicleList->at(i)->getPosition()).x / 5;
		float mapposY = Utils::instance().pxToGlmVec3(vehicleList->at(i)->getPosition()).z / 5;
		glm::vec2 mappos = { startPosX + mapposX, startPosY + mapposY };

		if (i == 0)
		{	
			if ((vehicleList->at(i)->getFrontVec().x)> 0)
			{
				imageList->at(i)->draw(green, mappos, glm::vec2(10.f, 10.f), 90 * (vehicleList->at(i)->getFrontVec().z + 1.f), glm::vec3(1.f, 1.f, 1.f));

			} else imageList->at(i)->draw(green, mappos, glm::vec2(10.f, 10.f), 360- 90 * (vehicleList->at(i)->getFrontVec().z + 1.f), glm::vec3(1.f, 1.f, 1.f));

		}
		else if (i == 1) {
			if ((vehicleList->at(i)->getFrontVec().x) > 0)
			{
				imageList->at(i)->draw(blue, mappos, glm::vec2(10.f, 10.f), 90 * (vehicleList->at(i)->getFrontVec().z + 1.f), glm::vec3(1.f, 1.f, 1.f));

			}
			else imageList->at(i)->draw(blue, mappos, glm::vec2(10.f, 10.f), 360 - 90 * (vehicleList->at(i)->getFrontVec().z + 1.f), glm::vec3(1.f, 1.f, 1.f));
		}
		else if (i == 2) {
			if ((vehicleList->at(i)->getFrontVec().x) > 0)
			{
				imageList->at(i)->draw(red, mappos, glm::vec2(10.f, 10.f), 90 * (vehicleList->at(i)->getFrontVec().z + 1.f), glm::vec3(1.f, 1.f, 1.f));

			}
			else imageList->at(i)->draw(red, mappos, glm::vec2(10.f, 10.f), 360 - 90 * (vehicleList->at(i)->getFrontVec().z + 1.f), glm::vec3(1.f, 1.f, 1.f));
		}
		else if (i == 3) {
			if ((vehicleList->at(i)->getFrontVec().x) > 0)
			{
				imageList->at(i)->draw(yellow, mappos, glm::vec2(10.f, 10.f), 90 * (vehicleList->at(i)->getFrontVec().z + 1.f), glm::vec3(1.f, 1.f, 1.f));

			}
			else imageList->at(i)->draw(yellow, mappos, glm::vec2(10.f, 10.f), 360 - 90 * (vehicleList->at(i)->getFrontVec().z + 1.f), glm::vec3(1.f, 1.f, 1.f));
		}

	}
	
	int playerNumber = GameManager::get().playerNumber;

	
	//Log::info("car x: {}", vehicleList->at(0)->getFrontVec().z);
	//Log::info("car z: {}", Utils::instance().pxToGlmVec3(vehicleList->at(0)->getPosition()).z);
	//Log::info("front vec {}", vehicleList->at(0)->getFrontVec().x, vehicleList->at(0)->getFrontVec().y);
	//vehicleList->at(0)->getFrontVec();
	//imageList->at(0)->draw(green, mappos, glm::vec2(20.f, 20.f), 180*(vehicleList->at(0)->getFrontVec().z + 1.f), glm::vec3(1.f, 1.f, 1.f));
	//imageList->at(1)->draw(blue, glm::vec2(1250, 50), glm::vec2(20.f, 20.f), 225, glm::vec3(1.f, 1.f, 1.f));
	//imageList->at(2)->draw(red, glm::vec2(1050, 50), glm::vec2(20.f, 20.f), 0, glm::vec3(1.f, 1.f, 1.f));
	//imageList->at(3)->draw(yellow, glm::vec2(1050, 150), glm::vec2(20.f, 20.f), 75, glm::vec3(1.f, 1.f, 1.f));
}