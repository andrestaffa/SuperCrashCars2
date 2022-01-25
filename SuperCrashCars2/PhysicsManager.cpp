#include "PhysicsManager.h"

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

PhysicsManager::PhysicsManager(const PxF32 timestep) : timestep(timestep) {

	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	PxU32 numWorkers = 1;
	gDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = VehicleFilterShader;
	sceneDesc.simulationEventCallback = &gEventCallback;

	gScene = gPhysics->createScene(sceneDesc);
	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));

	PxInitVehicleSDK(*gPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	//Create a plane to drive on.
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
	gScene->addActor(*gGroundPlane);
}

void PhysicsManager::simulate() {
	gScene->simulate(this->timestep);
	gScene->fetchResults(true);
}

void PhysicsManager::free() {
	PxCloseVehicleSDK();
	PX_RELEASE(gGroundPlane);
	PX_RELEASE(gMaterial);
	PX_RELEASE(gCooking);
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd) {
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
}

PxTriangleMesh* PhysicsManager::createTriangleMesh(const std::vector<PxVec3>& verts, const std::vector<PxU32>& indices) {

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = verts.size();
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = &verts[0];

	meshDesc.triangles.count = indices.size();
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = &indices[0];

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
	if (!status) return NULL;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	return gPhysics->createTriangleMesh(readBuffer);
}

PxRigidDynamic* PhysicsManager::createDynamic(PxTriangleMesh* triMesh) {
	PxRigidDynamic* meshActor = gPhysics->createRigidDynamic(PxTransform(PxIdentity));
	PxShape* meshShape;
	if (meshActor) {
		meshActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		PxTriangleMeshGeometry triGeom;
		triGeom.triangleMesh = triMesh;
		PxRigidActorExt::createExclusiveShape(*meshActor, triGeom, *gMaterial);
		gScene->addActor(*meshActor);
		return meshActor;
	}
	return NULL;
}

PhysicsManager::~PhysicsManager() {}