#include "Physics.h"
#include "Log.h"

Physics::Physics() {
	this->m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocatorCallback, m_defaultErrorCallback);
	if (!this->m_foundation) throw("PxCreateFoundation failed!");
	this->m_pvd = PxCreatePvd(*this->m_foundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	this->m_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	this->m_toleranceScale.length = 100;       
	this->m_toleranceScale.speed = 981;         
	this->m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *this->m_foundation, this->m_toleranceScale, true, this->m_pvd);

	physx::PxSceneDesc sceneDesc(this->m_physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	this->m_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = this->m_dispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	this->m_scene = this->m_physics->createScene(sceneDesc);

	physx::PxPvdSceneClient* pvdClient = this->m_scene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	this->m_material = this->m_physics->createMaterial(0.5f, 0.5f, 0.6f);
	physx::PxRigidStatic* groundPlane = PxCreatePlane(*this->m_physics, physx::PxPlane(0, 1, 0, 0), *this->m_material);
	this->m_scene->addActor(*groundPlane);
}

void Physics::createStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent) {
	using namespace physx;
	PxShape* shape = this->m_physics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *this->m_material);

	for (PxU32 i = 0; i < size; i++) {
		for (PxU32 j = 0; j < size - i; j++) {
			PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
			PxRigidDynamic* body = this->m_physics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 20.0f);
			this->m_scene->addActor(*body);
		}
	}

	shape->release();
}

physx::PxRigidDynamic* Physics::createDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxVec3& velocity) {

	physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(*this->m_physics, t, geometry, *this->m_material, 10.0f);

	dynamic->setMass(1.0f);
	dynamic->setAngularDamping(10.0f);
	dynamic->setLinearVelocity(velocity);

	this->m_scene->addActor(*dynamic);
	
	return dynamic;
}

void Physics::addObject(physx::PxRigidActor& rigidActor, GLMesh& mesh) {
	using namespace physx;

	const int MAX_NUM_ACTOR_SHAPES = 128;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];

	const PxU32 nbShapes = rigidActor.getNbShapes();
	PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
	rigidActor.getShapes(shapes, nbShapes);

	for (PxU32 i = 0; i < nbShapes; i++) {
		const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[i], rigidActor));
		const PxGeometryHolder h = shapes[i]->getGeometry();

		glm::mat4 TM = glm::make_mat4(&shapePose.column0.x);
		mesh.render(TM);
	}
}

void Physics::simulate() {
	this->m_scene->simulate(1.0f / 60.0f);
	this->m_scene->fetchResults(true);
}

Physics::~Physics() {}