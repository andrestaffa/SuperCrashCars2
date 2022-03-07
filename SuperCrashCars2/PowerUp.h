#pragma once

#include "PStatic.h"

enum class PowerUpType {
	eEMPTY = -1, // for when a car is holding no powerup
	eBOOST = 0,
	eHEALTH = 1,
	eDAMAGE = 2, // ??
	eSHIELD = 3,
	eJUMP = 4
};



class PowerUp : public PStatic {

public:
	PowerUp(PhysicsManager& pm, const Model& model, const PowerUpType& powerUpType, const PxVec3& position = PxVec3(0.0f), const PxQuat& rotation = PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f)));
	~PowerUp() {};

	void render();

	void destroy();
	PowerUpType getType();

	bool triggered = false;

private:
	PowerUpType m_powerUpType;

};