//********************************************************************************
//
// File: kernal.h
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for handling the scheduling of each task, aswell as calling other modules
// to perform the tasks.
//
//*********************************************************************************
#ifndef KERNEL_H
#define KERNEL_H

#include <FlightState.h>
#include <stdint.h>
#include "PWM.h"
#include "altitude.h"
#include "yaw.h"


//*********************************************************************************
// Handles the takeoff routine for the helicopter
//*********************************************************************************
void handleTakeOff(HelicopterMode *mode);


//*********************************************************************************
// Handles the hovering routine for the helicopter
//*********************************************************************************
void handleHovering(HelicopterMode *mode, uint8_t sliderState);


//*********************************************************************************
// Handles the landing routine for the helicopter
//*********************************************************************************
void handleLanding(HelicopterMode *mode);


//*********************************************************************************
// Schedules the state machine
//*********************************************************************************
void scheduler();


//*********************************************************************************
// Checks button inputs and adjusts setpoint accordingly
//*********************************************************************************
void checkButtons(void);


#endif /* KERNEL_H */
