#pragma once

#include <PxPhysicsAPI.h>
#include "PhysicsManager.h"
#include "Model.h"

class PStatic {

public:
	PStatic(PhysicsManager& pm, const Model& model, const PxVec3& position = PxVec3(0.0f), const PxQuat& rotation = PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f)));
	~PStatic() {};

	PxTransform getTransform() const;
	PxVec3 getPosition() const;
	PxRigidStatic* getRigidStatic() const;

	void render();
	void free();

private:
	PhysicsManager& m_pm;
	PxRigidStatic* m_static = NULL;
	Model m_model;

	PxRigidStatic* createStatic(const PxVec3& position, const PxQuat& rotation);

};