#include "PDynamic.h"

PDyanmic::PDyanmic(PhysicsManager& pm, const PxGeometry& g, const PxVec3& position, const PxQuat& rotation) : m_pm(pm) {
	
	this->m_dynamic = PxCreateDynamic(*pm.gPhysics, PxTransform(position, rotation), g, *pm.gMaterial, 10.0f);

	PxFilterData obstacleSimFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	this->setSimFilterData(this->m_dynamic, obstacleSimFilterData);

	this->m_dynamic->setMass(1000.0f);
	this->m_dynamic->setAngularDamping(10.0f);

	pm.gScene->addActor(*this->m_dynamic);

}

const PxTransform& PDyanmic::getTransform() const {
	return this->m_dynamic->getGlobalPose();
}

const PxVec3& PDyanmic::getPosition() const {
	return this->m_dynamic->getGlobalPose().p;
}

PxRigidDynamic& PDyanmic::getRigidDynamic() const {
	return *this->m_dynamic;
}

void PDyanmic::render(GLMesh& mesh) {
	
	const int MAX_NUM_ACTOR_SHAPES = 128;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];

	PxRigidActor* rigidActor = static_cast<PxRigidActor*>(this->m_dynamic);

	const PxU32 nbShapes = rigidActor->getNbShapes();

	PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
	rigidActor->getShapes(shapes, nbShapes);

	for (PxU32 i = 0; i < nbShapes; i++) {
		const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[i], *rigidActor));
		const PxGeometryHolder h = shapes[i]->getGeometry();

		glm::mat4 TM = glm::make_mat4(&shapePose.column0.x);
		mesh.render(TM);
	}
}

void PDyanmic::setSimFilterData(PxRigidActor* actor, PxFilterData& filterData) {
	const int MAX_NUM_ACTOR_SHAPES = 128;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	const PxU32 nbShapes = actor->getNbShapes();
	PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
	actor->getShapes(shapes, nbShapes);
	for (PxU32 i = 0; i < nbShapes; i++) shapes[i]->setSimulationFilterData(filterData);
}

void PDyanmic::free() {
	this->m_dynamic->release();
	this->m_dynamic = NULL;
}