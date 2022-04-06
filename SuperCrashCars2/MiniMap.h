#pragma once

#include "Log.h"
#include "PVehicle.h"

class MiniMap
{
public:
	MiniMap();
	MiniMap(int playerId, PVehicle& player);

	void displayMap(PVehicle& player, const std::vector<PVehicle*>& vehicleList);

private:
	int playerId;
	PxVec3 currentPos;
	glm::vec3 frontVec;

	std::vector<glm::vec3> oppoPos;
	std::vector<glm::vec3> relativePos;
	std::vector<PxVec3> oppoFrontVec;
	const std::vector<PVehicle> list;

	void updateMap(PVehicle& player, const std::vector<PVehicle*>& vehicleList);
};


