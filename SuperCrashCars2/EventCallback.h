#pragma once

#include <PxPhysicsAPI.h>
#include "Log.h"

using namespace physx;

class EventCallback : public PxSimulationEventCallback {

	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count);
	void onWake(PxActor** actors, PxU32 count);
	void onSleep(PxActor** actors, PxU32 count);
	void onTrigger(PxTriggerPair* pairs, PxU32 count);
	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32);
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);

};