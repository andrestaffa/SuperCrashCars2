#pragma once

#include "PhysicsManager.h"

#include "vehicle/PxVehicleUtil.h"
#include "SnippetVehicleSceneQuery.h"
#include "SnippetVehicleTireFriction.h"

#include "Log.h"

using namespace physx;
using namespace snippetvehicle;

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

class PVehicle {

public:

	PVehicle(PhysicsManager& pm, const PxVec3& position = PxVec3(0.0f, 0.0f, 0.0f), const PxQuat& quat = PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f)));
	~PVehicle();

	void accelerate(float throttle);
	void reverse(float throttle);
	void brake(float throttle);
	void turnLeft(float throttle);
	void turnRight(float throttle);
	void handbrake();
	void jump(PxVec3 impulse);
	
	const PxTransform& getTransform();
	const PxVec3& getPosition();
	PxRigidDynamic* getRigidDynamic();

	void removePhysics();
	void render(Model* tires, Model* body);

	void update();
	void free();

private:
	PxVehicleDrive4W* gVehicle4W = NULL;

	VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
	PxBatchQuery* gBatchQuery = NULL;
	PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;
	bool gIsVehicleInAir = true;

	PhysicsManager& m_pm;
	bool m_isFalling = false;
	bool m_isReversing = false;

	PxF32 gSteerVsForwardSpeedData[2 * 8] = {
		0.0f,		0.75f,
		5.0f,		0.75f,
		30.0f,		0.125f,
		120.0f,		0.1f,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32
	};
	PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable = PxFixedSizeLookupTable<8>(gSteerVsForwardSpeedData, 4);

	PxVehicleKeySmoothingData gKeySmoothingData = {
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
			6.0f,	//rise rate eANALOG_INPUT_BRAKE		
			6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
			2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
			2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
			10.0f,	//fall rate eANALOG_INPUT_BRAKE		
			10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
			5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
			5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
		}
	};

	PxVehiclePadSmoothingData gPadSmoothingData = {
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
			6.0f,	//rise rate eANALOG_INPUT_BRAKE		
			6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
			2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
			2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
			10.0f,	//fall rate eANALOG_INPUT_BRAKE		
			10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
			5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
			5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
		}
	};

	PxVehicleDrive4WRawInputData gVehicleInputData;

	VehicleDesc initVehicleDesc();
	void releaseAllControls();

};