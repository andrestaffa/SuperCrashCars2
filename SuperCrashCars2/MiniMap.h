#pragma once

#include "Log.h"
#include "PVehicle.h"
#include "Texture.h"
#include "Image.h"
#include "GameManager.h"
#include <string>
#include <math.h>
#include <string>
#define M_PI 3.14159265358979323846
class MiniMap
{
public:
	MiniMap();
	MiniMap(int playerId, PVehicle& player);

	void displayMap(PVehicle& player, const std::vector<PVehicle*>* vehicleList, std::vector<Image*> *imageList, int currentplayer);

private:
	Texture green = Texture("textures/green.png", GL_LINEAR);
	Texture blue = Texture("textures/blue.png", GL_LINEAR);
	Texture red = Texture("textures/red.png", GL_LINEAR);
	Texture yellow = Texture("textures/yellow.png", GL_LINEAR);
	Texture maptex = Texture("models/island/MariSS27.png", GL_LINEAR);
	int playerId;
	PxVec3 currentPos;
	glm::vec3 frontVec;
	std::vector<glm::vec3> oppoPos;
	std::vector<glm::vec3> relativePos;
	std::vector<PxVec3> oppoFrontVec;
	const std::vector<PVehicle> list;
	std::vector<Texture*> textureList;

	void updateMap(PVehicle& player, const std::vector<PVehicle*>& vehicleList);
};


