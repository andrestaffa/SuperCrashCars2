#include "PhysicsRenderer.h"

PhysicsRenderer::PhysicsRenderer() {
	/*this->m_material = this->m_physics->createMaterial(0.5f, 0.5f, 0.6f);
	physx::PxRigidStatic* groundPlane = PxCreatePlane(*this->m_physics, physx::PxPlane(0, 1, 0, 0), *this->m_material);
	this->m_scene->addActor(*groundPlane);*/
}

physx::PxRigidDynamic* PhysicsRenderer::createDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxVec3& velocity) {

	/*physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(*this->m_physics, t, geometry, *this->m_material, 10.0f);

	dynamic->setMass(1.0f);
	dynamic->setAngularDamping(10.0f);
	dynamic->setLinearVelocity(velocity);

	this->m_scene->addActor(*dynamic);

	return dynamic;*/
	return NULL;
}

void PhysicsRenderer::renderActor(physx::PxRigidActor& rigidActor, GLMesh& mesh) {
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

PhysicsRenderer::~PhysicsRenderer() {}