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

	// car1 hits car 0!

	if (car0 && car1) {

		PxVec3 car1Pos = car1->getGlobalPose().p;
		PxVec3 car0Pos = car0->getGlobalPose().p;
		PxVec3 launchVector = car0Pos - car1Pos;
		launchVector = launchVector.getNormalized();

		VehicleCollisionAttributes* attr = (VehicleCollisionAttributes*)car1->userData;
		attr->collided = true;

		//car1->setLinearVelocity(car1->getLinearVelocity() / 10.f);
		//car0->addForce(launchVector * 300000, PxForceMode::eIMPULSE);
		attr->forceToAdd = PxVec3(launchVector * 30000 * attr->collisionCoefficient);
		
		
		attr->collisionCoefficient = attr->collisionCoefficient + 0.5f;
		Log::debug("Damage car1 {}", attr->collisionCoefficient);

	}
}