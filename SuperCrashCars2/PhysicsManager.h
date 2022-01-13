#pragma once

#include <PxPhysicsAPI.h>
#include <vector>
#include "GLMesh.h"

#include "SnippetVehicleFilterShader.h"
#include "SnippetVehicleCreate.h"

using namespace physx;
using namespace snippetvehicle;

class PhysicsManager {

public:
	PhysicsManager(const PxF32 timestep);
	~PhysicsManager();

	PxDefaultAllocator gAllocator;
	PxDefaultErrorCallback gErrorCallback;
	PxFoundation* gFoundation = NULL;
	PxPhysics* gPhysics = NULL;
	PxDefaultCpuDispatcher* gDispatcher = NULL;
	PxScene* gScene = NULL;
	PxCooking* gCooking = NULL;
	PxMaterial* gMaterial = NULL;
	PxPvd* gPvd = NULL;
	PxRigidStatic* gGroundPlane = NULL;

	const PxF32 timestep;

	void simulate();
	void cleanupPhysics();

	PxRigidDynamic* instantiateDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(10));

	void renderActor(PxRigidActor& actor, GLMesh& mesh);
};