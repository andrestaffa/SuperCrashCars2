#pragma once

#include <PxPhysicsAPI.h>
#include "PhysicsManager.h"
#include "GLMesh.h"

class PStatic {

public:
	PStatic(PhysicsManager& pm, const PxGeometry& g, const PxVec3& position = PxVec3(0.0f), const PxQuat& rotation = PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f)));
	~PStatic() {};

	const PxTransform& getTransform();
	const PxVec3& getPosition();
	PxRigidStatic& getRigidStatic();

	void render(GLMesh& mesh);
	void free();

private:
	PhysicsManager& m_pm;
	PxRigidStatic* m_static = NULL;

	void setSimFilterData(PxRigidActor* actor, PxFilterData& filterData);

};