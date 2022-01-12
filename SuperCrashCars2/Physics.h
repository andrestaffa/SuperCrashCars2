#pragma once

#include <PxPhysicsAPI.h>
#include <vector>
#include "GLMesh.h"

class Physics {

public:
	Physics();
	~Physics();

	void createStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent);
	physx::PxRigidDynamic* createDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxVec3& velocity = physx::PxVec3(10));
	void addObject(physx::PxRigidActor& actor, GLMesh& mesh);

	void simulate();

private:
	physx::PxDefaultAllocator      m_defaultAllocatorCallback;
	physx::PxDefaultErrorCallback  m_defaultErrorCallback;
	physx::PxDefaultCpuDispatcher* m_dispatcher = NULL;
	physx::PxTolerancesScale       m_toleranceScale;

	physx::PxFoundation* m_foundation = NULL;
	physx::PxPhysics* m_physics = NULL;

	physx::PxMaterial* m_material = NULL;

	physx::PxPvd* m_pvd = NULL;
	physx::PxScene* m_scene = NULL;
};