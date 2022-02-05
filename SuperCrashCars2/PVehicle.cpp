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

	gVehicle4W = createVehicle4W(vehicleDesc, pm.gPhysics, pm.gCooking);

	PxTransform startTransform(PxVec3(0 + position.x, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f) + position.y, 0 + position.z), quat);
	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	pm.gScene->addActor(*gVehicle4W->getRigidDynamicActor());

	//Set the vehicle to rest in neutral.
	//Set the vehicle to use auto-gears.
	gVehicle4W->setToRestState();
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eNEUTRAL);
	gVehicle4W->mDriveDynData.setUseAutoGears(true);
	brake(1.0f);
}

void PVehicle::initVehicleModel() {
	
	switch (this->m_vehicleType) {
		case VehicleType::eJEEP:
		{
			this->m_chassis = Model("models/jeep/jeep.obj");
			this->m_chassis.translate(glm::vec3(-0.0f, -2.0f, 0.0f));
			this->m_chassis.scale(glm::vec3(1.5f, 1.5f, 1.2f));

			this->m_tires = Model("models/wheel/wheel.obj");
			this->m_tires.scale(glm::vec3(0.5f, 0.5f, 0.5f));
			break;
		}
		case VehicleType::eTOYOTA:
		{
			this->m_chassis = Model("models/toyota/toyota.obj");
			this->m_chassis.translate(glm::vec3(-0.125f, -2.5f, 0.20f));
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
	}
	 else if (this->m_vehicleType == VehicleType::eSHUCKLE) {
		 chassisMass = 800.0f;
		 chassisDims = PxVec3(2.0f, -5.0f, 3.5f);
	}
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.

	PxF32 wheelMass = 20.0f;
	PxF32 wheelRadius = 0.5f;
	PxF32 wheelWidth = 0.4f;

	if (this->m_vehicleType == VehicleType::eJEEP) {
		wheelMass = 20.0f;
		wheelRadius = 0.5f;
		wheelWidth = 0.4f;
	} else if (this->m_vehicleType == VehicleType::eTOYOTA) {
		wheelMass = 40.0f;
		wheelRadius = 0.6;
		wheelWidth = 0.5f;
	}
	else if (this->m_vehicleType == VehicleType::eSHUCKLE) {
		wheelMass = 15.0f;
		wheelRadius = 0.4;
		wheelWidth = 0.4f;
	}

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

	this->releaseAllControls();
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

void PVehicle::jump(PxVec3 impulse) {
	// not implemented
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
	return glm::normalize(glm::vec3(transformMat[0][1], transformMat[1][1], transformMat[2][1]));
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

void PVehicle::removePhysics() {	
	/*PxRigidActor* actor = this->gVehicle4W->getRigidDynamicActor();
	actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);*/
	if (!this->m_isFalling) {
		PxVec3 pos = this->gVehicle4W->getRigidDynamicActor()->getGlobalPose().p;
		this->gVehicle4W->getRigidDynamicActor()->setGlobalPose(PxTransform(pos + PxVec3(0.0f, -1.5f, 0.0f), this->gVehicle4W->getRigidDynamicActor()->getGlobalPose().q));
		this->m_isFalling = true;
	}

}

PVehicle::~PVehicle() {}