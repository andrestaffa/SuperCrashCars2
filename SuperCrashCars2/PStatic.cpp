#include "PStatic.h"

PStatic::PStatic(PhysicsManager& pm, const Model& model, const PxVec3& position, const PxQuat& rotation) : m_pm(pm), m_model(model) {

	this->m_static = this->createStatic(position, rotation);
	pm.gScene->addActor(*this->m_static);
}

PxTransform PStatic::getTransform() const {
	return this->m_static->getGlobalPose();
}

PxVec3 PStatic::getPosition() const {
	return this->m_static->getGlobalPose().p;
}

PxRigidStatic* PStatic::getRigidStatic() const {
	return this->m_static;
}

void PStatic::render() {
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
		this->m_model.draw(TM);
	}
}

PxRigidStatic* PStatic::createStatic(const PxVec3& position, const PxQuat& rotation) {
	std::vector<PxVec3> vertices;
	std::vector<PxU32> indices;
	for (const Mesh& mesh : this->m_model.getMeshData()) {
		for (const Vertex& vertex : mesh.m_vertices)
			vertices.push_back(PxVec3(vertex.Position.x, vertex.Position.y, vertex.Position.z));
		for (const unsigned int& index : mesh.m_indices)
			indices.push_back(index);
	}

	PxTriangleMesh* triMesh = this->m_pm.createTriangleMesh(vertices, indices);
	PxRigidStatic* staticActor = this->m_pm.gPhysics->createRigidStatic(PxTransform(position, rotation));
	if (staticActor) {
		PxFilterData obstacleSimFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
		PxTriangleMeshGeometry triGeom;
		triGeom.triangleMesh = triMesh;
		PxShape* meshShape = PxRigidActorExt::createExclusiveShape(*staticActor, triGeom, *this->m_pm.gMaterial);
		if (meshShape) meshShape->setSimulationFilterData(obstacleSimFilterData);
		return staticActor;
	}
	return nullptr;
}

void PStatic::free() {
	this->m_static->release();
	this->m_static = NULL;
}