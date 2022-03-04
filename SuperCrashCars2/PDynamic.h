#pragma once

#include <PxPhysicsAPI.h>
#include "PhysicsManager.h"
#include "Model.h"

class PDyanmic {

public:
	PDyanmic(PhysicsManager& pm, const Model& model, const PxVec3& position = PxVec3(0.0f), const PxQuat& rotation = PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f)));
	~PDyanmic() {};

	PxTransform getTransform() const;
	PxVec3 getPosition() const;
	PxRigidDynamic* getRigidDynamic() const;

	void render();
	void free();

protected:
	PhysicsManager& m_pm;
	PxRigidDynamic* m_dynamic = NULL;
	Model m_model;

	PxRigidDynamic* createDynamic(const PxVec3& position, const PxQuat& rotation);

};