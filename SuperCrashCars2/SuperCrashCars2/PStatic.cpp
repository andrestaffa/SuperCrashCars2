#include "PStatic.h"

PStatic::PStatic(PhysicsManager& pm, const PxGeometry& g, const PxVec3& position, const PxQuat& rotation) : m_pm(pm) {
	this->m_static = PxCreateStatic(*pm.gPhysics, PxTransform(position, rotation), g, *pm.gMaterial);

	PxFilterData obstacleSimFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	this->setSimFilterData(this->m_static, obstacleSimFilterData);

	pm.gScene->addActor(*this->m_static);
}

const PxTransform& PStatic::getTransform() {
	return this->m_static->getGlobalPose();
}

const PxVec3& PStatic::getPosition() {
	return this->m_static->getGlobalPose().p;
}

PxRigidStatic& PStatic::getRigidStatic() {
	return *this->m_static;
}

void PStatic::setSimFilterData(PxRigidActor* actor, PxFilterData& filterData) {
	const int MAX_NUM_ACTOR_SHAPES = 128;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	const PxU32 nbShapes = actor->getNbShapes();
	PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
	actor->getShapes(shapes, nbShapes);
	for (PxU32 i = 0; i < nbShapes; i++) shapes[i]->setSimulationFilterData(filterData);
}

void PStatic::render(GLMesh& mesh) {
	const int MAX_NUM_ACTOR_SHAPES = 128;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];

	PxRigidActor* rigidActor = static_cast<PxRigidActor*>(this->m_static);

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

void PStatic::free() {
	this->m_static->release();
	this->m_static = NULL;
}