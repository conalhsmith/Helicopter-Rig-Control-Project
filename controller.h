//*****************************************************************************
//
// File: controller.h
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for PID controllers for yaw and altitude control
//
// This module contains some code from the labs and lectures on learn
//
//*****************************************************************************

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <float.h>
#include <stdint.h>

//*****************************************************************************
// PID controller for yaw control
// Code template taken from lectures on learn
//*****************************************************************************
float YawPIDController(float setpoint, float current_value);


//*****************************************************************************
// PID controller for altitude control
// Code template taken from lectures on learn
//*****************************************************************************
float AltitudePIDController(float setpoint, float current_value);


//*****************************************************************************
// Returns Yaw Duty Cycle
//*****************************************************************************
uint32_t getYawDuty(void);


//*****************************************************************************
// Returns Altitude Duty Cycle
//*****************************************************************************
uint32_t getAltitudeDuty(void);


#endif /* CONTROLLER_H_ */
