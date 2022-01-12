#pragma once

#include <PxPhysicsAPI.h>
#include <vector>
#include "GLMesh.h"

class PhysicsRenderer {

public:
	PhysicsRenderer();
	~PhysicsRenderer();

	physx::PxRigidDynamic* createDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxVec3& velocity = physx::PxVec3(10));
	void renderActor(physx::PxRigidActor& actor, GLMesh& mesh);

};