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

#define PWM_MAX_DUTY 80
#define PWM_MAIN_MIN_DUTY 40
#define PWM_TAIL_MIN_DUTY 15

#define PWM_HOVER_DUTY 52

// PID constants for yaw control
#define YAW_KP 5
#define YAW_KI 0.5
#define YAW_KD 0.5

// PID constants for altitude control
#define ALTITUDE_KP 12
#define ALTITUDE_KI 0.5
#define ALTITUDE_KD 0.5

// Variables to store previous error terms for PID calculation
#define DELTA_T (1 / 150)

static float Aoutput = 0;


//*****************************************************************************
// PID controller for yaw control
//*****************************************************************************
float YawPIDController(float setpoint, float current_value)
{
    static float YI = 0;
    static float prev_yaw_reading = 0;
    // Calculate error
    float error = setpoint - current_value;

    // Proportional term
    float P = YAW_KP * error;

    // Integral term
    float dI = YAW_KI * error * DELTA_T;

    // Derivative term
    float D = YAW_KD * (prev_yaw_reading - current_value) / DELTA_T;

    // Update previous error
    prev_yaw_reading = current_value;

    // Calculate PID output
    float Youtput = P + (YI+dI) + D + 0.8 * Aoutput;

    if (Youtput > PWM_MAX_DUTY){
        Youtput = PWM_MAX_DUTY;
    } else if (Youtput < PWM_TAIL_MIN_DUTY) {
        Youtput = PWM_TAIL_MIN_DUTY;
    } else {
        YI = (YI+dI);
    }

    return Youtput;
}

//*****************************************************************************
// PID controller for altitude control
//*****************************************************************************
float AltitudePIDController(float setpoint, float current_value)
{
    static float AI = 0;
    static float prev_alt_reading = 0;
    // Calculate error
    float error = setpoint - current_value;

    // Proportional term
    float P = ALTITUDE_KP * error;

    // Integral term
    float dI = ALTITUDE_KI * error * DELTA_T;

    // Derivative term
    float D = ALTITUDE_KD * (prev_alt_reading - current_value) / DELTA_T;

    // Update previous error
    prev_alt_reading = current_value;

    // Calculate PID output
    Aoutput = P + (AI+dI) + D + PWM_HOVER_DUTY;

    if (Aoutput > PWM_MAX_DUTY){
        Aoutput = PWM_MAX_DUTY;
    } else if (Aoutput < PWM_MAIN_MIN_DUTY) {
        Aoutput = PWM_MAIN_MIN_DUTY;
    } else {
        AI = (AI+dI);
    }

    return Aoutput;
}

