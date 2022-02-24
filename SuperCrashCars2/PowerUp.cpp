#include "PowerUp.h"

PowerUp::PowerUp(PhysicsManager& pm, const Model& model, const PowerUpType& powerUpType, const PxVec3& position, const PxQuat& rotation) :
	PStatic(pm, model, position, rotation) 
{
	this->m_static->userData = this;
	this->m_powerUpType = powerUpType;

	const int MAX_NUM_ACTOR_SHAPES = 128;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	PxRigidActor* rigidActor = static_cast<PxRigidActor*>(this->m_static);
	const PxU32 nbShapes = rigidActor->getNbShapes();
	rigidActor->getShapes(shapes, nbShapes);

	for (int i = 0; i < nbShapes; i++) {
		shapes[i]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shapes[i]->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}

}

PowerUpType PowerUp::getType() {
	return this->m_powerUpType;
}

void PowerUp::render() {

	PxRigidActor* rigidActor = static_cast<PxRigidActor*>(this->m_static);
	if (!rigidActor) return;

	const int MAX_NUM_ACTOR_SHAPES = 128;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];

	const PxU32 nbShapes = rigidActor->getNbShapes();

	PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
	rigidActor->getShapes(shapes, nbShapes);

	for (PxU32 i = 0; i < nbShapes; i++) {
		const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[i], *rigidActor));
		const PxGeometryHolder h = shapes[i]->getGeometry();
		
		glm::mat4 TM = glm::make_mat4(&shapePose.column0.x);

		// rotate
		this->m_model.rotate(glm::radians(2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shapes[i]->setLocalPose(PxTransform(shapes[i]->getLocalPose().p, PxQuat(this->m_model.getAngle(), PxVec3(0.0f, 1.0f, 0.0f))));

		this->m_model.draw(TM);
	}
}

void PowerUp::destroy() {

	const int MAX_NUM_ACTOR_SHAPES = 128;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	PxRigidActor* rigidActor = static_cast<PxRigidActor*>(this->m_static);
	const PxU32 nbShapes = rigidActor->getNbShapes();
	rigidActor->getShapes(shapes, nbShapes);

	for (int i = 0; i < nbShapes; i++) {
		PxConvexMeshGeometry x;
		shapes[i]->getConvexMeshGeometry(x);
		x.scale.scale = PxVec3(PX_MESH_SCALE_MIN);
		shapes[i]->setGeometry(x);
		this->m_static->detachShape(*shapes[i]);
	}

	this->free();
}

