#pragma once

#include "PxPhysicsAPI.h"

// Initialization
extern void initPhysics();
extern void cleanupPhysics();
extern void stepPhysics(const physx::PxF32 timestamp);

// Object Instantiation
extern physx::PxVehicleDrive4W* instantiateVehicle();


// Handling
extern void accelerate(float throttle);
extern void reverse(float throttle);
extern void brake(float throttle);
extern void turnLeft(float throttle);
extern void turnRight(float throttle);
extern void handbrake(float throttle);
extern void releaseAllControls();

