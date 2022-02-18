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

		// figure out who hit who
		float car0Mag = car0->getLinearVelocity().magnitudeSquared();
		float car1Mag = car1->getLinearVelocity().magnitudeSquared();

		// the faster car is the attacker, the slower car is the victim
		PxRigidDynamic* attacker;
		PxRigidDynamic* victim;

		if (car0Mag > car1Mag) {
			attacker = car0;
			victim = car1;
		} 
		else {
			attacker = car1;
			victim = car0;
		}
		
		// getting naive launch vector
		PxVec3 attackerPos = attacker->getGlobalPose().p;
		PxVec3 victimPos = victim->getGlobalPose().p;
		PxVec3 launchVector = victimPos - attackerPos;
		launchVector = launchVector.getNormalized();

		VehicleCollisionAttributes* victimAttr = (VehicleCollisionAttributes*)victim->userData;
		victimAttr->collided = true;

		//car1->setLinearVelocity(car1->getLinearVelocity() / 10.f);
		//car0->addForce(launchVector * 300000, PxForceMode::eIMPULSE);
		victimAttr->forceToAdd = PxVec3(launchVector * 500000 * victimAttr->collisionCoefficient);
		
		
		victimAttr->collisionCoefficient = victimAttr->collisionCoefficient + 0.5f;


	}
}