#include "PVehicle.h"

using namespace physx;

PVehicle::PVehicle(int id, PhysicsManager& pm, const VehicleType& vehicleType, PlayerOrAI carType, const PxVec3& position, const PxQuat& quat) : m_pm(pm), m_vehicleType(vehicleType) {
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

	this->m_powerUpPocket = PowerUpType::eEMPTY;

	PxTransform startTransform(PxVec3(0 + position.x, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f) + position.y, 0 + position.z), quat);
	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	pm.gScene->addActor(*gVehicle4W->getRigidDynamicActor());

	this->initVehicleCollisionAttributes();
	gVehicle4W->getRigidDynamicActor()->userData = this;

	this->getRigidDynamic()->setMaxAngularVelocity(4.0f);
	this->getRigidDynamic()->setAngularDamping(0.2f);

	m_lives = 3;
	m_state = VehicleState::ePLAYING;
	m_shieldUseTimestamp = steady_clock::now();

	this->carid = id;
	this->m_carType = carType;
	this->accelerating = false;

	m_shieldSphere = Model("models/sphere/sphere.obj");
	m_shieldSphere.setPosition(Utils::instance().pxToGlmVec3(this->getPosition()));
	m_shieldSphere.scale(glm::vec3(0.37f, 0.37f, 0.37f));

	//Set the vehicle to rest in neutral.
	//Set the vehicle to use auto-gears.
	gVehicle4W->setToRestState();
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eNEUTRAL);
	gVehicle4W->mDriveDynData.setUseAutoGears(true);
	brake(1.0f);
}
#pragma region physx
void PVehicle::initVehicleCollisionAttributes() {
	this->vehicleAttr = VehicleCollisionAttributes();
	this->vehicleAttr.collisionCoefficient = 1.0f;
	this->vehicleAttr.collided = false;

	this->vehicleAttr.targetVehicle = nullptr;
	this->vehicleAttr.targetPowerup = nullptr;
	this->vehicleAttr.reachedTarget = false;

	this->vehicleAttr.forceToAdd = PxVec3(0.0f, 0.0f, 0.0f);
	this->vehicleAttr.targetTimestamp = steady_clock::now();
}
void PVehicle::initVehicleModel() {
	
	switch (this->m_vehicleType) {
		case VehicleType::eAVA_GREEN:
		{
			this->m_chassis = Model("models/ava_car_green/ava_car.obj");
			//this->m_chassis.translate(glm::vec3(-0.125f, -1.15f, 0.20f));
			this->m_chassis.translate(glm::vec3(0.0f, -1.25f, 0.0f));
			this->m_chassis.scale(glm::vec3(0.95f, 1.f, 1.45f));

			this->m_tires = Model("models/wheel/ava_wheel.obj");
			this->m_tires.scale(glm::vec3(0.95f));

			break;
		}
		case VehicleType::eAVA_BLUE:
		{
			this->m_chassis = Model("models/ava_car_blue/ava_car.obj");
			//this->m_chassis.translate(glm::vec3(-0.125f, -1.15f, 0.20f));
			this->m_chassis.translate(glm::vec3(0.0f, -1.25f, 0.0f));
			this->m_chassis.scale(glm::vec3(0.95f, 1.f, 1.45f));

			this->m_tires = Model("models/wheel/ava_wheel.obj");
			this->m_tires.scale(glm::vec3(0.95f));

			break;
		}
		case VehicleType::eAVA_RED:
		{
			this->m_chassis = Model("models/ava_car_red/ava_car.obj");
			//this->m_chassis.translate(glm::vec3(-0.125f, -1.15f, 0.20f));
			this->m_chassis.translate(glm::vec3(0.0f, -1.25f, 0.0f));
			this->m_chassis.scale(glm::vec3(0.95f, 1.f, 1.45f));

			this->m_tires = Model("models/wheel/ava_wheel.obj");
			this->m_tires.scale(glm::vec3(0.95f));

			break;
		}
		case VehicleType::eAVA_YELLOW:
		{
			this->m_chassis = Model("models/ava_car_yellow/ava_car.obj");
			//this->m_chassis.translate(glm::vec3(-0.125f, -1.15f, 0.20f));
			this->m_chassis.translate(glm::vec3(0.0f, -1.25f, 0.0f));
			this->m_chassis.scale(glm::vec3(0.95f, 1.f, 1.45f));

			this->m_tires = Model("models/wheel/ava_wheel.obj");
			this->m_tires.scale(glm::vec3(0.95f));

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

	//if (this->m_vehicleType == VehicleType::eAVA_GREEN) { // all vehicles will be the same
		chassisMass = 8000.0f;
		chassisDims = PxVec3(4.0f, 2.0f, 7.5f);
	//}

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
	//if (this->m_vehicleType == VehicleType::eAVA_GREEN) 
		wheelMass = 40.0f;

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
void PVehicle::updatePhysics() {
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

	// update sphere position.
	m_shieldSphere.setPosition(Utils::instance().pxToGlmVec3(this->gVehicle4W->getRigidDynamicActor()->getGlobalPose().p));

	// other updates over time

	this->releaseAllControls();

	// boosting
	this->regainBoost();
	this->regainJump();
	this->regainFlash();
}
void PVehicle::free() {
	PX_RELEASE(gBatchQuery);
	gVehicleSceneQueryData->free(this->m_pm.gAllocator);
	PX_RELEASE(gFrictionPairs);

	gVehicle4W->getRigidDynamicActor()->release();
	gVehicle4W->free();
}
void PVehicle::releaseAllControls() {
	gVehicleInputData.setAnalogAccel(0.0f);
	gVehicleInputData.setAnalogSteer(0.0f);
	gVehicleInputData.setAnalogBrake(0.0f);
	gVehicleInputData.setAnalogHandbrake(0.0f);
}
#pragma endregion
#pragma region movement
void PVehicle::accelerate(float throttle) {
	if (this->gVehicle4W->getRigidDynamicActor()->getLinearVelocity().magnitude() >= 30.0f) return;
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	gVehicleInputData.setAnalogAccel(throttle);
}
void PVehicle::reverse(float throttle) {
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
	gVehicleInputData.setAnalogAccel(throttle);
}
void PVehicle::brake(float throttle) {
	gVehicleInputData.setAnalogBrake(throttle);
}
void PVehicle::turnLeft(float throttle) {
	gVehicleInputData.setAnalogSteer(throttle);
	this->rotateXAxis(-0.35f);
	/*PxVec3 vel = this->getRigidDynamic()->getLinearVelocity();
	float mag = vel.magnitude();
	PxVec3 front = Utils::instance().glmToPxVec3(this->getFrontVec());
	this->getRigidDynamic()->setLinearVelocity(front * mag * 0.75f + vel * 0.25f);*/
	this->getRigidDynamic()->setAngularVelocity(this->getRigidDynamic()->getAngularVelocity() * 0.99f);
}
void PVehicle::turnRight(float throttle) {
	gVehicleInputData.setAnalogSteer(-throttle);
	this->rotateXAxis(0.35f);
	/*PxVec3 vel = this->getRigidDynamic()->getLinearVelocity();
	float mag = vel.magnitude();
	PxVec3 front = Utils::instance().glmToPxVec3(this->getFrontVec());
	this->getRigidDynamic()->setLinearVelocity(front * mag * 0.75f + vel * 0.25f);*/
	this->getRigidDynamic()->setAngularVelocity(this->getRigidDynamic()->getAngularVelocity() * 0.99f);
}
void PVehicle::handbrake() {
	gVehicleInputData.setAnalogHandbrake(1.0f);
	this->getRigidDynamic()->setAngularVelocity(this->getRigidDynamic()->getAngularVelocity() * 1.01f);
}
void PVehicle::rotateYAxis(float amount) {
	glm::vec3 rightVec = this->getRightVec();
	this->getRigidDynamic()->addTorque(-0.05f * amount * PxVec3(rightVec.x, rightVec.y, rightVec.z), PxForceMode::eVELOCITY_CHANGE);
}
void PVehicle::rotateXAxis(float amount) {
	glm::vec3 upVec = this->getUpVec();
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
			PxVec3 front = Utils::instance().glmToPxVec3(this->getFrontVec());
			this->getRigidDynamic()->setLinearVelocity(front * mag * 0.75f + vel * 0.25f);
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
	if (this->vehicleParams.boost < 100 && difftime(time(0), this->vehicleParams.boostCooldown) > 0.2f && !this->getVehicleInAir()) this->vehicleParams.boost++;
}
void PVehicle::jump() {
	if (this->vehicleParams.canJump) {
		this->vehicleParams.canJump = false;
		PxVec3 vel = this->getRigidDynamic()->getLinearVelocity();
		if (vel.y < 0) this->getRigidDynamic()->setLinearVelocity(PxVec3(vel.x, 0.f, vel.z));
		this->getRigidDynamic()->addForce(PxVec3(0.0, 15.0f, 0.0), PxForceMode::eVELOCITY_CHANGE);
		this->vehicleParams.jumpCooldown = time(0);
		AudioManager::get().playSound(SFX_JUMP_NORMAL, Utils::instance().pxToGlmVec3(this->getPosition()), 0.45f);
	}
}
void PVehicle::regainJump() {
	if (difftime(time(0), this->vehicleParams.jumpCooldown) > 1.0f && !this->getVehicleInAir()) this->vehicleParams.canJump = true;
}



#pragma endregion
#pragma region getters
PxMat44 PVehicle::getTransform() const {
	PxMat44 mat = this->gVehicle4W->getRigidDynamicActor()->getGlobalPose();
	return mat.getTranspose();
}
PxVec3 PVehicle::getPosition() const {
	return this->gVehicle4W->getRigidDynamicActor()->getGlobalPose().p;
}
PxRigidDynamic* PVehicle::getRigidDynamic() const {
	return this->gVehicle4W->getRigidDynamicActor();
}
glm::vec3 PVehicle::getFrontVec() {
	PxMat44 transformMat = PxTransform(this->getTransform());
	return glm::normalize(glm::vec3(transformMat[0][2], transformMat[1][2], transformMat[2][2]));
}
glm::vec3 PVehicle::getUpVec() {
	PxMat44 transformMat = PxTransform(this->getTransform());
	return glm::normalize(glm::vec3(transformMat[0][1], transformMat[1][1], transformMat[2][1]));
}
glm::vec3 PVehicle::getRightVec() {
	PxMat44 transformMat = PxTransform(this->getTransform());
	return glm::normalize(glm::vec3(transformMat[0][0], transformMat[1][0], transformMat[2][0]));
}
bool PVehicle::getVehicleInAir() {
	return this->gIsVehicleInAir;
}
PowerUpType PVehicle::getPocket() const {
	return this->m_powerUpPocket;
}
#pragma endregion
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

		if (i < 4) this->m_tires.draw(TM);
		else  this->m_chassis.draw(TM);

		//if (i >= 4) this->m_chassis.draw(TM);

	}
}

void PVehicle::reset() {
	this->getRigidDynamic()->setGlobalPose(PxTransform(this->m_startingPosition, PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f))));
	this->getRigidDynamic()->setLinearVelocity(PxVec3(0.f));
	this->getRigidDynamic()->setAngularVelocity(PxVec3(0.f));
	this->vehicleParams.boost = 100;
	//this->m_lives = 3;
}

void PVehicle::flashWhite() {
	this->vehicleParams.flashWhite = 1.0f;
	this->vehicleParams.flashDuration = time(0);
}

void PVehicle::regainFlash() {
	if (this->vehicleParams.flashWhite > 0.0f) this->vehicleParams.flashWhite -= 0.03;
	if (this->vehicleParams.flashWhite < 0.0f) this->vehicleParams.flashWhite = 0.0f;
}

void PVehicle::updateState() {
	time_point now = steady_clock::now();

	switch (this->m_state) {
	case VehicleState::ePLAYING:

		if (this->getPosition().y < -100.f) {
			Log::debug("dead:");
			this->m_state = VehicleState::eRESPAWNING;
			deathTimestamp = steady_clock::now();
			this->m_lives--;
			AudioManager::get().playSound(SFX_DEATH, Utils::instance().pxToGlmVec3(this->getPosition()), 0.9f);
			this->vehicleAttr.collisionCoefficient = 0.0f;
			if (this->m_lives == 0) {
				this->m_state = VehicleState::eOUTOFLIVES;

			}
			else {
				this->m_state = VehicleState::eRESPAWNING;
			}
		}
		break;
	case VehicleState::eDEAD:
		break;
	case VehicleState::eRESPAWNING:

		reset();
		if (duration_cast<seconds>(now - deathTimestamp) > seconds(2)) {
			this->m_state = VehicleState::ePLAYING; // after 2 seconds passed since death, respawn
		}
		break;
	case VehicleState::eOUTOFLIVES:
		GameManager::get().winner = this->carid;
		break;
	}

	switch (m_shieldState){
	case ShieldPowerUpState::eINACTIVE:
		break;
	case ShieldPowerUpState::eACTIVE:
		if (duration_cast<seconds>(now - m_shieldUseTimestamp) > seconds(5)) m_shieldState = ShieldPowerUpState::eEXPIRING;
		break;
	case ShieldPowerUpState::eEXPIRING:
		if (duration_cast<seconds>(now - m_shieldUseTimestamp) > seconds(8)) m_shieldState = ShieldPowerUpState::eLAST_SECOND;

		break;
	case ShieldPowerUpState::eLAST_SECOND:
		if (duration_cast<seconds>(now - m_shieldUseTimestamp) > seconds(10)) m_shieldState = ShieldPowerUpState::eINACTIVE;
		break;
	} 

	// update audio 
	
	// not ready yet

	//AudioManager::get().updateCarPos(Utils::instance().pxToGlmVec3(this->getPosition()), this->carid);

}


#pragma region powerups
void PVehicle::pickUpPowerUp(PowerUp* p) {
	// powerups that apply as soon as you collect them
	switch (p->getType()) {
	case PowerUpType::eBOOST:
		this->vehicleParams.boost += 50;
		break;

	case PowerUpType::eHEALTH:
		this->applyHealthPowerUp();
		break;
	}

		// powerups that are collected and used later
	if (this->m_powerUpPocket == PowerUpType::eEMPTY) {
		switch (p->getType()) {
			case PowerUpType::eJUMP: 
				this->m_powerUpPocket = PowerUpType::eJUMP;
				break;
			
			case PowerUpType::eSHIELD: 
				this->m_powerUpPocket = PowerUpType::eSHIELD;
				break;
			
		}
	}

}

void PVehicle::usePowerUp() {
	PxVec3 vel = this->getRigidDynamic()->getLinearVelocity();
	switch (this->m_powerUpPocket) {
	case PowerUpType::eJUMP:

		if (vel.y < 0) this->getRigidDynamic()->setLinearVelocity(PxVec3(vel.x, 0.f, vel.z));
		this->getRigidDynamic()->addForce(PxVec3(0.0, 30.0f, 0.0), PxForceMode::eVELOCITY_CHANGE);
		AudioManager::get().playSound(SFX_JUMP_MEGA, Utils::instance().pxToGlmVec3(this->getPosition()), 0.45f);
		break;

	case PowerUpType::eSHIELD:
		this->m_shieldState = ShieldPowerUpState::eACTIVE;
		m_shieldUseTimestamp = steady_clock::now();
		this->m_powerUpPocket = PowerUpType::eSHIELD;
		break;

	}
	this->m_powerUpPocket = PowerUpType::eEMPTY;

}

void PVehicle::applyHealthPowerUp() {
	this->vehicleAttr.collisionCoefficient -= 0.3f;
	if (this->vehicleAttr.collisionCoefficient < 0.f) {
		this->vehicleAttr.collisionCoefficient = 0.f;
	}
}
#pragma endregion

#pragma region ai

void PVehicle::driveTo(const PxVec3& targetPos, PVehicle* targetVehicle, PowerUp* targetPowerUp) {
	//this->vehicleParams.boost = 100;
	bool isPowerUp = false;
	
	if (targetVehicle) {
		this->vehicleAttr.targetVehicle = (PVehicle*)targetVehicle;
		this->vehicleAttr.targetPowerup = nullptr;
		isPowerUp = false;
	} else if (targetPowerUp) {
		this->vehicleAttr.targetPowerup = (PowerUp*)targetPowerUp;
		this->vehicleAttr.targetVehicle = nullptr;
		isPowerUp = true;
	}

	// detecting edge
	PxVec3 updatedPos = targetPos;
	if (abs(this->getPosition().x) > 230.f || abs(this->getPosition().z) > 230.f) {
		//if (this->getVehicleInAir() && this->getRigidDynamic()->getLinearVelocity().magnitude() < 50.0f) {

			PxMat44 transformMat = PxTransform(this->getTransform());
			PxVec3 newFront = (PxVec3(0.f, 40, 0.f) - this->getPosition()).getNormalized();
			transformMat[0][2] = newFront.x;
			transformMat[1][2] = newFront.y;
			transformMat[2][2] = newFront.z;


			//this->boost();
			if (this->vehicleParams.boost > 0) {
				this->getRigidDynamic()->addForce(PxVec3(newFront.x, newFront.y, newFront.z) * 0.5f, PxForceMode::eVELOCITY_CHANGE);
				this->vehicleParams.boost--;
			}


		
		if (this->getRigidDynamic()->getLinearVelocity().magnitude() > 15.0f) this->brake(1);
		updatedPos = PxVec3(0.0f);
	}

	PxVec2 p = PxVec2(updatedPos.x, updatedPos.z) - PxVec2(this->getPosition().x, this->getPosition().z);
	glm::vec2 relativeVec = glm::vec2(p.x, p.y);

	float angle = glm::orientedAngle(glm::normalize(glm::vec2(this->getFrontVec().x, this->getFrontVec().z)), glm::normalize(relativeVec));
	float degrees = angle * 180.0f / PxPi;
	
	if (degrees < 5.0f && degrees > -5.0f) {
		this->accelerate(1.0f);
	} else if (degrees < 0) {
		this->turnLeft(1.0f);
		this->accelerate(0.5f);
	} else if (degrees > 0) {
		this->turnRight(1.0f);
		this->accelerate(0.5f);
	}

	if (glm::length(relativeVec) < 20.f && !isPowerUp && (this->vehicleParams.boost > 40)) this->boost();

	// vehicle logic
	if (!isPowerUp && targetVehicle && (targetVehicle->m_state == VehicleState::eDEAD || targetVehicle->m_state == VehicleState::eOUTOFLIVES) ) this->vehicleAttr.reachedTarget = true;

	// power up logic
	if (isPowerUp && !targetPowerUp->active) this->vehicleAttr.reachedTarget = true;
	if (glm::length(relativeVec) < 20.0f && isPowerUp) this->jump();

}
#pragma endregion
void PVehicle::setCar_tpye(PlayerOrAI carType) {
	this->m_carType = carType;
}

PVehicle::~PVehicle() {}