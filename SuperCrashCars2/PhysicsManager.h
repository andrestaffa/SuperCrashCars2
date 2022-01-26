#pragma once

#include <PxPhysicsAPI.h>
#include "Model.h"

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

	Model m_groundModel;

	void simulate();
	void free();

	void drawGround();

	PxTriangleMesh* createTriangleMesh(const std::vector<PxVec3>& verts, const std::vector<PxU32>& indices);
	PxConvexMesh* createConvexMesh(const std::vector<PxVec3>& verts);
};