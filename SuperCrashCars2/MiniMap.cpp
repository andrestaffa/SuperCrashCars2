#include "MiniMap.h"

MiniMap::MiniMap() {
	// Image rendering
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


void MiniMap::displayMap(PVehicle& player, const std::vector<PVehicle*>& vehicleList, std::vector<Image*> *imageList) {
	//updateMap(player, vehicleList);
	//Image image1 = Image(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	//Image image2 = Image(Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	
	Texture green("textures/green.png", GL_LINEAR); // Just a placeholder/test image
	Texture blue("textures/blue.png", GL_LINEAR); // Just a placeholder/test image
	Texture red("textures/red.png", GL_LINEAR); // Just a placeholder/test image
	Texture yellow("textures/yellow.png", GL_LINEAR); // Just a placeholder/test image
	//Texture& texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color
	//image.draw(texture, glm::vec2(Utils::instance().SCREEN_WIDTH - 200.f, 0), glm::vec2(200.f, 200.f), 0.f, glm::vec3(1.f, 1.f, 1.f));

	 imageList->at(4)->draw(green, glm::vec2(1130, 30), glm::vec2(200.f, 200.f), 0, glm::vec3(1.f, 1.f, 1.f));
	 imageList->at(0)->draw(green, glm::vec2(1130, 30), glm::vec2(20.f, 20.f), 0, glm::vec3(1.f, 1.f, 1.f));
	 imageList->at(1)->draw(blue, glm::vec2(1250, 50), glm::vec2(20.f, 20.f), 225, glm::vec3(1.f, 1.f, 1.f));
	 imageList->at(2)->draw(red, glm::vec2(1050, 50), glm::vec2(20.f, 20.f), 0, glm::vec3(1.f, 1.f, 1.f));
	 imageList->at(3)->draw(yellow, glm::vec2(1050, 150), glm::vec2(20.f, 20.f), 75, glm::vec3(1.f, 1.f, 1.f));

	//Calcalate the relative positon.
	//use draw func to draw a arrow at specific pos
}