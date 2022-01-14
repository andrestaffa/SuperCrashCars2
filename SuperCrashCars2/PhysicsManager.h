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

	PxConvexMesh* createConvexMesh(const PxVec3* verts, const PxU32 numVerts);
	
	PxRigidStatic* createStatic(const PxTransform& t, const PxGeometry& geometry);
	PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry);

	void renderActor(PxRigidActor& actor, GLMesh& mesh);

private:
	void setSimFilterData(PxRigidActor* actor, PxFilterData& filterData);

};