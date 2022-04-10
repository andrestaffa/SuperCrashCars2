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
	float startPosX = Utils::instance().SCREEN_WIDTH - 110.f;
	float startPosY = Utils::instance().SCREEN_HEIGHT - 630.f;

	//Single player
	for (size_t i = 0; i < 4; i++)
	{
		float mapposX = Utils::instance().pxToGlmVec3(vehicleList->at(i)->getPosition()).x / 5;
		float mapposY = Utils::instance().pxToGlmVec3(vehicleList->at(i)->getPosition()).z / 5;
		glm::vec2 mappos = { startPosX + mapposX, startPosY + mapposY };
		if (i == 0)
		{
			if ((vehicleList->at(i)->getFrontVec().x) > 0)
			{
				imageList->at(i)->draw(green, mappos, glm::vec2(10.f, 10.f), 90 * (vehicleList->at(i)->getFrontVec().z + 1.f), glm::vec3(1.f, 1.f, 1.f));

			}
			else imageList->at(i)->draw(green, mappos, glm::vec2(10.f, 10.f), 360 - 90 * (vehicleList->at(i)->getFrontVec().z + 1.f), glm::vec3(1.f, 1.f, 1.f));

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
	imageList->at(4)->draw(maptex, glm::vec2(startPosX - 90, 0), glm::vec2(200.f, 200.f), 0.f, glm::vec3(1.f, 1.f, 1.f));
	return;
}