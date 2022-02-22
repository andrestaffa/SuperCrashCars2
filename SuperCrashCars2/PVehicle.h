#pragma once

#include "PhysicsManager.h"

#include "vehicle/PxVehicleUtil.h"
#include "SnippetVehicleSceneQuery.h"
#include "SnippetVehicleTireFriction.h"

#include "Model.h"
#include "Log.h"

using namespace physx;
using namespace snippetvehicle;

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

enum class VehicleType {
	eJEEP = 0,
	eTOYOTA = 1,
	eSHUCKLE = 2
};

struct VehicleCollisionAttributes {
	float collisionCoefficient;
	bool collided;
	PxVec3 forceToAdd;
};

struct VehicleParams {
	
	// keyboard throttle
	float k_throttle = 1.0f;

	// jumping
	float jumpCoefficient;
	time_t jumpCooldown;
	bool canJump = true;

	// boosting
	float boostCoefficient;
	time_t boostCooldown;
	int boost = 100;

};

class PVehicle {

public:

	PVehicle(PhysicsManager& pm, const VehicleType& vehicleType, const PxVec3& position = PxVec3(0.0f, 0.0f, 0.0f), const PxQuat& quat = PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f)));
	~PVehicle();

	void accelerate(float throttle);
	void reverse(float throttle);
	void brake(float throttle);
	void turnLeft(float throttle);
	void turnRight(float throttle);
	void handbrake();
	void rotateYAxis(float amount);
	void rotateXAxis(float amount);
	void boost();
	void regainBoost();
	void jump();
	void regainJump();
	void reset();
	
	PxTransform getTransform() const;
	PxVec3 getPosition() const;
	PxRigidDynamic* getRigidDynamic() const;
	glm::vec3 getFrontVec();
	glm::vec3 getUpVec();
	glm::vec3 getRightVec();

	void render();

	void update();
	void free();

	bool getVehicleInAir();

	VehicleParams vehicleParams;

private:
	PxVehicleDrive4W* gVehicle4W = NULL;

	VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
	PxBatchQuery* gBatchQuery = NULL;
	PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;
	bool gIsVehicleInAir = true;

	PhysicsManager& m_pm;
	bool m_isFalling = false;
	bool m_isReversing = false;

	VehicleType m_vehicleType;

	PxVec3 m_startingPosition;

	Model m_chassis;
	Model m_tires;

	VehicleCollisionAttributes m_attr;

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

	void initVehicleModel();
	VehicleDesc initVehicleDesc();
	void adjustConvexCollisionMesh(const PxVec3& chassis_tran, const PxVec3& chassis_scale, const PxVec3& wheel_tran, const PxVec3& wheel_scale);
	void releaseAllControls();

	void initVehicleCollisionAttributes();

};