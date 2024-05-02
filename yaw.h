//********************************************************************************
//
// File: yaw.h
//
// Authors: Conal Smith
//          Adam Mason
//
// Module to handle finite state machine for yaw and convert to degrees.
//
//*********************************************************************************


#ifndef YAW_H_
#define YAW_H_

#include <stdint.h>
#include <stdbool.h>

//*********************************************************************************
// Finite state machine for encoder position.
//*********************************************************************************
void YawIntHandler(void);

//*********************************************************************************
// Inititiates the interupt and required ports for then yaw module.
//*********************************************************************************
void initYaw(void);

//*********************************************************************************
// Converts the yaw value into degrees
//*********************************************************************************
int16_t getYawDegrees(void);

//*********************************************************************************
// Resets yaw value based if it reaches max yaw in either direction.
//*********************************************************************************
void ResetYaw(void);

#endif /* YAW_H_ */
