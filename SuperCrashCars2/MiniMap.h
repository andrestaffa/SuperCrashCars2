#pragma once

#include "Log.h"
#include "PVehicle.h"

class MiniMap
{
public:
	MiniMap(int playerId);
	void updateMap(int playerId, std::vector<PVehicle*> vehicleList);
	void displayMap(int PlayerId);
private:
	PxVec3 currentPos;
	PxVec3 frontVec;
	std::vector<PxVec3> oppoPos;
	std::vector<PxVec3> oppoFrontVec;
};

