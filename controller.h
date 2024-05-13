//*****************************************************************************
//
// File: controller.h
//
// Authors: [Your Name]
//
// Module for PID controllers for yaw and altitude control
//
//*****************************************************************************

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <float.h>
//*****************************************************************************
// PID controller for yaw control
//*****************************************************************************
float YawPIDController(float setpoint, float current_value);

//*****************************************************************************
// PID controller for altitude control
//*****************************************************************************
float AltitudePIDController(float setpoint, float current_value);

#endif /* CONTROLLER_H_ */
