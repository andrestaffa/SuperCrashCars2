#pragma once

#include <PxPhysicsAPI.h>
#include <vector>

#include "EventCallback.h"
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
	EventCallback gEventCallback;

	const PxF32 timestep;

	void simulate();
	void free();

	PxTriangleMesh* createTriangleMesh(const std::vector<PxVec3>& verts, const std::vector<PxU32>& indices);
	PxRigidDynamic* createDynamic(PxTriangleMesh* triMesh);

};