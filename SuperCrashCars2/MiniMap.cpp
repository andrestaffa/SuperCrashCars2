#include "MiniMap.h"

MiniMap::MiniMap() {

}

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


void MiniMap::displayMap(PVehicle& player, const std::vector<PVehicle*>& vehicleList) {
	updateMap(player, vehicleList);
	//Calcalate the relative positon.
	//use draw func to draw a arrow at specific pos.
	int i = 0;
	for (auto it : oppoPos) {

		std::vector<glm::vec3> oppoPos;

		//Log::info(i + "oppoPos: " + it.x);
		/*std::vector<glm::vec3> relativePos;
		std::vector<PxVec3> oppoFrontVec;*/
	}


}