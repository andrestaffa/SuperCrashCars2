#include "PDynamic.h"

PDyanmic::PDyanmic(PhysicsManager& pm, const Model& model, const PxVec3& position, const PxQuat& rotation) : m_pm(pm), m_model(model) {
	
	this->m_dynamic = this->createDynamic(position, rotation);
	this->m_dynamic->setAngularDamping(10.0f);
	pm.gScene->addActor(*this->m_dynamic);

}

PxTransform PDyanmic::getTransform() const {
	return this->m_dynamic->getGlobalPose();
}

PxVec3 PDyanmic::getPosition() const {
	return this->m_dynamic->getGlobalPose().p;
}

PxRigidDynamic* PDyanmic::getRigidDynamic() const {
	return this->m_dynamic;
}

void PDyanmic::render() {
	
	PxRigidActor* rigidActor = static_cast<PxRigidActor*>(this->m_dynamic);
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
		this->m_model.draw(TM);
	}
}

PxRigidDynamic* PDyanmic::createDynamic(const PxVec3& position, const PxQuat& rotation) {
	std::vector<PxVec3> vertices;
	for (const Mesh& mesh : this->m_model.getMeshData())
		for (const Vertex& vertex : mesh.m_vertices)
			vertices.push_back(PxVec3(vertex.Position.x, vertex.Position.y, vertex.Position.z));

	PxConvexMesh* convexMesh = this->m_pm.createConvexMesh(vertices);
	PxRigidDynamic* meshActor = this->m_pm.gPhysics->createRigidDynamic(PxTransform(position, rotation));
	PxFilterData obstacleSimFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);

	if (meshActor) {
		PxConvexMeshGeometry convexGeom = PxConvexMeshGeometry(convexMesh);
		convexGeom.meshFlags = PxConvexMeshGeometryFlag::eTIGHT_BOUNDS;
		PxShape* convexShape = PxRigidActorExt::createExclusiveShape(*meshActor, convexGeom, *this->m_pm.gMaterial);
		if (convexShape) convexShape->setSimulationFilterData(obstacleSimFilterData);
		return meshActor;
	}

	return nullptr;
}

void PDyanmic::free() {
	if (!this->m_dynamic) return;
	this->m_dynamic->release();
	this->m_dynamic = NULL;
}