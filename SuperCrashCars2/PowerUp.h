#pragma once

#include "PStatic.h"

enum class PowerUpType {
	eBOOST = 0,
	eHEALTH = 1,
	eDAMAGE = 2
};

class PowerUp : public PStatic {

public:
	PowerUp(PhysicsManager& pm, const Model& model, const PowerUpType& powerUpType, const PxVec3& position = PxVec3(0.0f), const PxQuat& rotation = PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f)));
	~PowerUp() {};

	void render();

	void destroy();
	PowerUpType getType();

private:
	PowerUpType m_powerUpType;

};