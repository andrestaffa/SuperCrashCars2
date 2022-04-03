#pragma once

#include "PhysicsManager.h"

#include "vehicle/PxVehicleUtil.h"
#include "SnippetVehicleSceneQuery.h"
#include "SnippetVehicleTireFriction.h"

#include "Model.h"
#include "Log.h"
#include "Utils.h"
#include "GameManager.h"

#include <glm/gtx/vector_angle.hpp>

#include "PowerUp.h"

#include <chrono>



using namespace physx;
using namespace snippetvehicle;
using namespace std::chrono;

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

enum class VehicleType {
	eAVA = 1
};

enum class PlayerOrAI {
	ePLAYER,
	eAI
};

enum class VehicleState {
	ePLAYING,
	eRESPAWNING,
	eDEAD, 
	eOUTOFLIVES
};

struct VehicleCollisionAttributes {
	float collisionCoefficient;
	bool collided;
	
	void* targetVehicle;
	void* targetPowerup;
	bool reachedTarget;

	PxVec3 forceToAdd;
	PxVec3 collisionMidpoint;
};

struct VehicleParams {

	// keyboard throttle
	float k_throttle = 1.0f;

	// jumping
	bool canJump = true;
	time_t jumpCooldown;

	// boosting
	bool boosting = false;
	time_t boostCooldown;
	int boost = 100;

	// visual effects
	float flashWhite = 0.0f;
	time_t flashDuration;

};

class PVehicle {

public:

	PVehicle(int id, PhysicsManager& pm, const VehicleType& vehicleType, PlayerOrAI carType, const PxVec3& position = PxVec3(0.0f, 0.0f, 0.0f), const PxQuat& quat = PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f)));
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
	void flashWhite();
	void regainFlash();
	void reset();
	void updateSound();

	PxMat44 getTransform() const;
	PxVec3 getPosition() const;
	PxRigidDynamic* getRigidDynamic() const;
	PowerUpType getPocket() const;
	glm::vec3 getFrontVec();
	glm::vec3 getUpVec();
	glm::vec3 getRightVec();
	
	Model m_shieldSphere;

	void render();

	void updatePhysics();
	void free();

	bool getVehicleInAir();
	void pickUpPowerUp(PowerUp* p);
	void usePowerUp();
	void applyHealthPowerUp();
	ShieldPowerUpState m_shieldState;
	time_point<steady_clock> m_shieldUseTimestamp;

	VehicleCollisionAttributes vehicleAttr;
	VehicleParams vehicleParams;
	int m_lives;

	void checkDeath();
	void updateState();
	VehicleState m_state;
	time_point<steady_clock> deathTimestamp;
	int carid;
	PowerUpType m_powerUpPocket; // bag

	// AI
	void driveTo(const PxVec3& targetPos, PVehicle* targetVehicle, PowerUp* targetPowerUp);
	PlayerOrAI m_carType;

private:
	PxVehicleDrive4W* gVehicle4W = NULL;

	VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
	PxBatchQuery* gBatchQuery = NULL;
	PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;
	bool gIsVehicleInAir = true;

	PhysicsManager& m_pm;
	bool m_isFalling = false;

	VehicleType m_vehicleType;

	PxVec3 m_startingPosition;

	Model m_chassis;
	Model m_tires;




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
