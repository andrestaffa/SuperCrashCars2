#include "PVehicle.h"

using namespace physx;

PVehicle::PVehicle(PhysicsManager& pm, const PxVec3& position, const PxQuat& quat) : m_pm(pm) {
	//Create the batched scene queries for the suspension raycasts.
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, pm.gAllocator);
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, pm.gScene);

	//Create the friction table for each combination of tire and surface type.
	gFrictionPairs = createFrictionPairs(pm.gMaterial);

	VehicleDesc vehicleDesc = initVehicleDesc();

	gVehicle4W = createVehicle4W(vehicleDesc, pm.gPhysics, pm.gCooking);

	PxTransform startTransform(PxVec3(0 + position.x, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f) + position.y, 0 + position.z), quat);
	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	pm.gScene->addActor(*gVehicle4W->getRigidDynamicActor());

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	gVehicle4W->setToRestState();
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eNEUTRAL);
	gVehicle4W->mDriveDynData.setUseAutoGears(true);
	brake(1.0f);
}

VehicleDesc PVehicle::initVehicleDesc() {
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 1500.0f;
	const PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 20.0f;
	const PxF32 wheelRadius = 0.5f;
	const PxF32 wheelWidth = 0.4f;
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
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

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
	// 
}

void PVehicle::releaseAllControls() {
	gVehicleInputData.setAnalogAccel(0.0f);
	gVehicleInputData.setAnalogSteer(0.0f);
	gVehicleInputData.setAnalogBrake(0.0f);
	gVehicleInputData.setAnalogHandbrake(0.0f);
}

const PxTransform& PVehicle::getTransform() {
	return this->gVehicle4W->getRigidDynamicActor()->getGlobalPose();
}

const PxVec3& PVehicle::getPosition() {
	return this->gVehicle4W->getRigidDynamicActor()->getGlobalPose().p;
}

PxRigidDynamic* PVehicle::getRigidDynamic() {
	return this->gVehicle4W->getRigidDynamicActor();
}

void PVehicle::render(Model* tires, Model* body) {
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
			if (tires)
				tires->draw(TM);
		} else {
			PxTransform rotTransform = PxShapeExt::getGlobalPose(*shapes[i], *rigidActor);
			float bodyAngle = PxPi - rotTransform.q.getAngle();
			glm::vec3 front = glm::vec3(sin(bodyAngle), 0.0f, -cos(bodyAngle));
			front = glm::normalize(front);
			if (body)
				body->draw(TM);
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