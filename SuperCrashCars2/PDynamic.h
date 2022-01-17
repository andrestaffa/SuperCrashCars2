#pragma once

#include <PxPhysicsAPI.h>
#include "PhysicsManager.h"
#include "GLMesh.h"

class PDyanmic {

public:
	PDyanmic(PhysicsManager& pm, const PxGeometry& g, const PxVec3& position = PxVec3(0.0f), const PxQuat& rotation = PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f)));
	~PDyanmic() {};

	const PxTransform& getTransform();
	const PxVec3& getPosition();
	PxRigidDynamic& getRigidDynamic();

	void render(GLMesh& mesh);
	void free();

private:
	PhysicsManager& m_pm;
	PxRigidDynamic* m_dynamic = NULL;

	void setSimFilterData(PxRigidActor* actor, PxFilterData& filterData);

};