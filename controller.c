//*****************************************************************************
//
// File: controller.c
//
// Authors: [Your Name]
//
// Module for PID controllers for yaw and altitude control
//
//*****************************************************************************

#include "controller.h"
#include <float.h>

// PID constants for yaw control
#define YAW_KP 1.0
#define YAW_KI 0.1
#define YAW_KD 0.01

// PID constants for altitude control
#define ALTITUDE_KP 1.0
#define ALTITUDE_KI 0.1
#define ALTITUDE_KD 0.01

// Variables to store previous error terms for PID calculation
static float prev_yaw_error = 0.0;
static float prev_altitude_error = 0.0;

//*****************************************************************************
// PID controller for yaw control
//*****************************************************************************
float YawPIDController(float setpoint, float current_value)
{
    // Calculate error
    float error = setpoint - current_value;

    // Proportional term
    float P = YAW_KP * error;

    // Integral term
    float I = YAW_KI * error;

    // Derivative term
    float D = YAW_KD * (error - prev_yaw_error);

    // Update previous error
    prev_yaw_error = error;

    // Calculate PID output
    float output = P + I + D;

    return output;
}

//*****************************************************************************
// PID controller for altitude control
//*****************************************************************************
float AltitudePIDController(float setpoint, float current_value)
{
    // Calculate error
    float error = setpoint - current_value;

    // Proportional term
    float P = ALTITUDE_KP * error;

    // Integral term
    float I = ALTITUDE_KI * error;

    // Derivative term
    float D = ALTITUDE_KD * (error - prev_altitude_error);

    // Update previous error
    prev_altitude_error = error;

    // Calculate PID output
    float output = P + I + D;

    return output;
}

