#pragma once

#include "PStatic.h"

#include <chrono>
using namespace std::chrono;

enum class PowerUpType {
	eEMPTY = -1, // for when a car is holding no powerup
	eBOOST = 0,
	eHEALTH = 1,
	eDAMAGE = 2, // ??
	eSHIELD = 3,
	eJUMP = 4
};

enum class ShieldPowerUpState { // states of a shield powerup
	eINACTIVE = 0,
	eACTIVE = 1,
	eEXPIRING = 2,
	eLAST_SECOND = 3
};



class PowerUp : public PStatic {

public:
	PowerUp(PhysicsManager& pm, const Model& model, const PowerUpType& powerUpType, const PxVec3& position = PxVec3(0.0f), const PxQuat& rotation = PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f)));
	~PowerUp() {};

	void render();

	void destroy();
	void collect();
	void tryRespawn();
	void forceRespawn();
	PowerUpType getType();

	bool triggered, active;
	time_point<steady_clock> triggeredTimestamp;	

private:
	PowerUpType m_powerUpType;
	PxVec3 m_startingPosition;
	
};