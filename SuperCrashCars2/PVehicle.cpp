#include "PVehicle.h"

using namespace physx;

PVehicle::PVehicle(PhysicsManager& pm, const VehicleType& vehicleType, const PxVec3& position, const PxQuat& quat) : m_pm(pm), m_vehicleType(vehicleType) {
	//Create the batched scene queries for the suspension raycasts.
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, pm.gAllocator);
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, pm.gScene);

	//Create the friction table for each combination of tire and surface type.
	gFrictionPairs = createFrictionPairs(pm.gMaterial);

	this->initVehicleModel(); // maybe change where this is called.

	VehicleDesc vehicleDesc = initVehicleDesc();

	std::vector<PxVec3> chassisVertices;
	std::vector<PxVec3> wheelVertices;
	for (const Mesh& mesh : this->m_chassis.getMeshData()) for (const Vertex& vertex : mesh.m_vertices) chassisVertices.push_back(PxVec3(vertex.Position.x, vertex.Position.y, vertex.Position.z));
	for (const Mesh& mesh : this->m_tires.getMeshData()) for (const Vertex& vertex : mesh.m_vertices) wheelVertices.push_back(PxVec3(vertex.Position.x, vertex.Position.y, vertex.Position.z));
	gVehicle4W = createVehicle4W(vehicleDesc, chassisVertices, wheelVertices, pm.gPhysics, pm.gCooking);
	const PxVec3& chassis_pos = PxVec3(this->m_chassis.getPosition().x, this->m_chassis.getPosition().y, this->m_chassis.getPosition().z);
	const PxVec3& chassis_scale = PxVec3(this->m_chassis.getScale().x, this->m_chassis.getScale().y, this->m_chassis.getScale().z);
	const PxVec3& wheel_pos = PxVec3(this->m_tires.getPosition().x, this->m_tires.getPosition().y, this->m_tires.getPosition().z);
	const PxVec3& wheel_scale = PxVec3(this->m_tires.getScale().x, this->m_tires.getScale().y, this->m_tires.getScale().z);
	this->m_startingPosition = position;
	this->adjustConvexCollisionMesh(chassis_pos, chassis_scale, wheel_pos, wheel_scale);

	PxTransform startTransform(PxVec3(0 + position.x, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f) + position.y, 0 + position.z), quat);
	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	pm.gScene->addActor(*gVehicle4W->getRigidDynamicActor());

	this->initVehicleCollisionAttributes();
	gVehicle4W->getRigidDynamicActor()->userData = &this->m_attr;

	this->getRigidDynamic()->setMaxAngularVelocity(4.0f);
	this->getRigidDynamic()->setAngularDamping(0.1f);
	

	//Set the vehicle to rest in neutral.
	//Set the vehicle to use auto-gears.
	gVehicle4W->setToRestState();
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eNEUTRAL);
	gVehicle4W->mDriveDynData.setUseAutoGears(true);
	brake(1.0f);
}

// the inits
void PVehicle::initVehicleCollisionAttributes() {
	this->m_attr = VehicleCollisionAttributes();
	this->m_attr.collisionCoefficient = 1.0f;
	this->m_attr.collided = false;
	this->m_attr.forceToAdd = PxVec3(0.0f, 0.0f, 0.0f);
}
void PVehicle::initVehicleModel() {
	
	switch (this->m_vehicleType) {
		case VehicleType::eJEEP:
		{
			this->m_chassis = Model("models/jeep/jeep.obj");
			this->m_chassis.translate(glm::vec3(-0.0f, -1.0f, 0.0f));
			this->m_chassis.scale(glm::vec3(1.5f, 1.5f, 1.2f));

			this->m_tires = Model("models/wheel/wheel.obj");
			this->m_tires.scale(glm::vec3(0.5f, 0.5f, 0.5f));
			break;
		}
		case VehicleType::eTOYOTA:
		{
			this->m_chassis = Model("models/toyota/toyota.obj");
			this->m_chassis.translate(glm::vec3(-0.125f, -1.15f, 0.20f));
			this->m_chassis.scale(glm::vec3(1.65f, 1.5f, 1.2f));

			this->m_tires = Model("models/wheel/wheel.obj");
			this->m_tires.scale(glm::vec3(0.75f, 0.625f, 0.625f));

			break;
		}
		case VehicleType::eSHUCKLE:
		{
			this->m_chassis = Model("models/shuckle/shuckle.obj");
			this->m_chassis.translate(glm::vec3(0.125f, 2.5f, -0.20f));
			this->m_chassis.scale(glm::vec3(1.65f, 1.5f, 1.2f));

			this->m_tires = Model("models/wheel/wheel.obj");
			this->m_tires.scale(glm::vec3(0.75f, 0.625f, 0.625f));

			break;
		}
		default:
			break;
	}

}
VehicleDesc PVehicle::initVehicleDesc() {
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.

	PxF32 chassisMass = 1500.0f;
	PxVec3 chassisDims = PxVec3(2.5f, 2.0f, 5.0f);

	if (this->m_vehicleType == VehicleType::eJEEP) {
		chassisMass = 1500.0f;
		chassisDims = PxVec3(2.5f, 2.0f, 5.0f);
	} else if (this->m_vehicleType == VehicleType::eTOYOTA) {
		chassisMass = 8000.0f;
		chassisDims = PxVec3(3.0f, 2.0f, 7.5f);
	} else if (this->m_vehicleType == VehicleType::eSHUCKLE) {
		chassisMass = 2000.0f;
		chassisDims = PxVec3(2.0f, -5.0f, 3.5f);
	}

	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.15f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.

	const PxF32 wheelRadius = 0.5f;
	const PxF32 wheelWidth = 0.4f;

	PxF32 wheelMass = 20.0f;
	if (this->m_vehicleType == VehicleType::eJEEP) wheelMass = 20.0f;
	else if (this->m_vehicleType == VehicleType::eTOYOTA) wheelMass = 40.0f;
	else if (this->m_vehicleType == VehicleType::eSHUCKLE) wheelMass = 15.0f;

	const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 4;

	VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = this->m_pm.gMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = this->m_pm.gMaterial;
	vehicleDesc.wheelSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	return vehicleDesc;
}

void PVehicle::adjustConvexCollisionMesh(const PxVec3& chassis_tran, const PxVec3& chassis_scale, const PxVec3& wheel_tran, const PxVec3& wheel_scale) {
	const int MAX_NUM_ACTOR_SHAPES = 128;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	PxRigidActor* rigidActor = static_cast<PxRigidActor*>(gVehicle4W->getRigidDynamicActor());
	const PxU32 nbShapes = rigidActor->getNbShapes();
	rigidActor->getShapes(shapes, nbShapes);
	shapes[4]->setLocalPose(PxTransform(chassis_tran, PxQuat(PxIdentity)));
	PxConvexMeshGeometry c;
	shapes[4]->getConvexMeshGeometry(c);
	c.scale.scale = chassis_scale;
	shapes[4]->setGeometry(c);
	for (int i = 0; i < 4; i++) {
		shapes[i]->setLocalPose(PxTransform(wheel_tran, PxQuat(PxIdentity)));
		PxConvexMeshGeometry x;
		shapes[i]->getConvexMeshGeometry(x);
		x.scale.scale = wheel_scale;
		shapes[i]->setGeometry(x);
	}
}

void PVehicle::update() {
	PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, this->m_pm.timestep, gIsVehicleInAir, *gVehicle4W);

	//Raycasts.
	PxVehicleWheels* vehicles[1] = { gVehicle4W };
	PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

	//Vehicle update.
	const PxVec3 grav = this->m_pm.gScene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
	PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, gVehicle4W->mWheelsSimData.getNbWheels()} };
	PxVehicleUpdates(this->m_pm.timestep, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

	//Work out if the vehicle is in the air.
	gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);

	// other updates over time

	this->releaseAllControls();

	// boosting
	this->regainBoost();
	this->regainJump();
}

void PVehicle::free() {
	PX_RELEASE(gBatchQuery);
	gVehicleSceneQueryData->free(this->m_pm.gAllocator);
	PX_RELEASE(gFrictionPairs);

	gVehicle4W->getRigidDynamicActor()->release();
	gVehicle4W->free();
}


void PVehicle::accelerate(float throttle) {
	this->m_isReversing = false;
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	gVehicleInputData.setAnalogAccel(throttle);
}
void PVehicle::reverse(float throttle) {
	this->m_isReversing = true;
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
	gVehicleInputData.setAnalogAccel(throttle);
}
void PVehicle::brake(float throttle) {
	PxVec3 velocity = this->gVehicle4W->getRigidDynamicActor()->getLinearVelocity();
	if (velocity.magnitude() <= 0.05f || this->m_isReversing) reverse(throttle);
	else gVehicleInputData.setAnalogBrake(throttle);
}
void PVehicle::turnLeft(float throttle) {
	gVehicleInputData.setAnalogSteer(throttle);
}
void PVehicle::turnRight(float throttle) {
	gVehicleInputData.setAnalogSteer(-throttle);
}
void PVehicle::handbrake() {
	gVehicleInputData.setAnalogHandbrake(1.0f);
}

void PVehicle::rotateYAxis(float amount) {
	glm::vec3 rightVec = this->getRightVec();
	this->getRigidDynamic()->addTorque(0.05f * amount * PxVec3(-rightVec.x, rightVec.y, rightVec.z), PxForceMode::eVELOCITY_CHANGE);
}

void PVehicle::rotateXAxis(float amount) {
	// TODO: make a started rotating bool and fix the vector after rotation has started
	// so that the car does not wobble
	glm::vec3 upVec = this->getUpVec();
	//this->getRigidDynamic()->addForce(0.5 * PxVec3(-upVec.x, upVec.y, upVec.z), PxForceMode::eVELOCITY_CHANGE);
	this->getRigidDynamic()->addTorque(-0.04f * amount * PxVec3(upVec.x, upVec.y, upVec.z), PxForceMode::eVELOCITY_CHANGE);
}

void PVehicle::boost() {
	if (this->vehicleParams.boost > 0) {
		// stuff to do if vehicle just started boosting
		if (!this->vehicleParams.boosting) {
			// if the vehicle just started boosting and not already boosting
			// we will get magnitude of current velocity and reapply it in the direction of the car's front vector
			PxVec3 vel = this->getRigidDynamic()->getLinearVelocity();
			float mag = vel.magnitude();
			PxMat44 transformMat = PxTransform(this->getTransform());
			this->getRigidDynamic()->setLinearVelocity(PxVec3(-transformMat[0][2], transformMat[1][2], transformMat[2][2]) * mag * 1.0f + vel * 0.0f);
			Log::debug("Front: {}, {}, {}, oldv: {}, {}, {}, newVel: {}, {}, {}, mag: {}", 
				-transformMat[0][2], transformMat[1][2], transformMat[2][2], 
				vel.x, vel.y, vel.z, 
				this->getRigidDynamic()->getLinearVelocity().x, this->getRigidDynamic()->getLinearVelocity().y, this->getRigidDynamic()->getLinearVelocity().z, mag);
			// then, dampen rotation on start of boost
			this->getRigidDynamic()->setAngularVelocity(this->getRigidDynamic()->getAngularVelocity() * 0.7f);
		}
		glm::vec3 frontVec = this->getFrontVec();
		this->getRigidDynamic()->addForce(PxVec3(frontVec.x, frontVec.y, frontVec.z) * 0.5f, PxForceMode::eVELOCITY_CHANGE);
		this->vehicleParams.boost--;
		this->vehicleParams.boostCooldown = time(0);
	}
}

void PVehicle::regainBoost() {
	if (this->vehicleParams.boost < 100) if (difftime(time(0), this->vehicleParams.boostCooldown) > 0.2f) this->vehicleParams.boost++;
}


void PVehicle::jump() {
	if (this->vehicleParams.canJump) {
		this->vehicleParams.canJump = false;
		this->getRigidDynamic()->addForce(PxVec3(0.0, 16.0f, 0.0), PxForceMode::eVELOCITY_CHANGE);
		this->vehicleParams.jumpCooldown = time(0);
	}
}

void PVehicle::regainJump() {
	if (difftime(time(0), this->vehicleParams.jumpCooldown) > 2.0f) this->vehicleParams.canJump = true;
}

void PVehicle::reset(){
	this->getRigidDynamic()->setGlobalPose(PxTransform(this->m_startingPosition, PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f))));
	this->getRigidDynamic()->setLinearVelocity(PxVec3(0.f));
	this->getRigidDynamic()->setAngularVelocity(PxVec3(0.f));
	this->vehicleParams.boost = 200;
}

void PVehicle::releaseAllControls() {
	gVehicleInputData.setAnalogAccel(0.0f);
	gVehicleInputData.setAnalogSteer(0.0f);
	gVehicleInputData.setAnalogBrake(0.0f);
	gVehicleInputData.setAnalogHandbrake(0.0f);
}

PxTransform PVehicle::getTransform() const {
	return this->gVehicle4W->getRigidDynamicActor()->getGlobalPose();
}

PxVec3 PVehicle::getPosition() const {
	return this->gVehicle4W->getRigidDynamicActor()->getGlobalPose().p;
}

PxRigidDynamic* PVehicle::getRigidDynamic() const {
	return this->gVehicle4W->getRigidDynamicActor();
}

glm::vec3 PVehicle::getFrontVec() {
	PxMat44 transformMat = PxTransform(this->getTransform());
	return glm::normalize(glm::vec3(-transformMat[0][2], transformMat[1][2], transformMat[2][2]));
}

glm::vec3 PVehicle::getUpVec() {
	PxMat44 transformMat = PxTransform(this->getTransform());
	return glm::normalize(glm::vec3(-transformMat[0][1], transformMat[1][1], transformMat[2][1]));
}

glm::vec3 PVehicle::getRightVec() {
	PxMat44 transformMat = PxTransform(this->getTransform());
	return glm::normalize(glm::vec3(transformMat[0][0], transformMat[1][0], transformMat[2][0]));
}

void PVehicle::render() {
	const int MAX_NUM_ACTOR_SHAPES = 128;
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];

	PxRigidActor* rigidActor = static_cast<PxRigidActor*>(gVehicle4W->getRigidDynamicActor());

	const PxU32 nbShapes = rigidActor->getNbShapes();

	PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
	rigidActor->getShapes(shapes, nbShapes);

	for (PxU32 i = 0; i < nbShapes; i++) {
		const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[i], *rigidActor));
		const PxGeometryHolder h = shapes[i]->getGeometry();

		glm::mat4 TM = glm::make_mat4(&shapePose.column0.x);

		// 0 -> front-right tire
		// 1 -> front-left tire
		// 2 -> back-right tire
		// 3 -> back-left tire
		// 4 -> body

		if (i < 4) {
			this->m_tires.draw(TM);
		} else {
			PxTransform rotTransform = PxShapeExt::getGlobalPose(*shapes[i], *rigidActor);
			float bodyAngle = PxPi - rotTransform.q.getAngle();
			glm::vec3 front = glm::vec3(sin(bodyAngle), 0.0f, -cos(bodyAngle));
			front = glm::normalize(front);
			this->m_chassis.draw(TM);
		}

	}
}

bool PVehicle::getVehicleInAir() {
	return this->gIsVehicleInAir;
}

PVehicle::~PVehicle() {}