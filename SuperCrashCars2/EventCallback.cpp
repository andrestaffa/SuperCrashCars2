#include "EventCallback.h"
#include "PVehicle.h"

void EventCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
void EventCallback::onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
void EventCallback::onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
void EventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count) { PX_UNUSED(pairs); PX_UNUSED(count); }
void EventCallback::onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}

void EventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {

	PxRigidActor* actor0 = pairHeader.actors[0];
	PxRigidActor* actor1 = pairHeader.actors[1];

	PxRigidDynamic* car0 = reinterpret_cast<PxRigidDynamic*>(actor0);
	PxRigidDynamic* car1 = reinterpret_cast<PxRigidDynamic*>(actor1);

	if (car0 && car1) {

		CarAttributes* attr = (CarAttributes*)car1->userData;
		car0->addForce(PxVec3(20000.0f), PxForceMode::eIMPULSE);
	
	}

}