//*****************************************************************************
//
// File: controller.c
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for PID controllers for yaw and altitude control
//
//*****************************************************************************

#include "controller.h"
#include <float.h>
#include <stdint.h>


//*********************************************************************************
// Constants
//*********************************************************************************
#define PWM_MAX_DUTY 80
#define PWM_MAIN_MIN_DUTY 40
#define PWM_TAIL_MIN_DUTY 10
#define PWM_HOVER_DUTY 52
#define DELTA_T (1 / 150)
#define K_COUPLING 0.8


//*********************************************************************************
// PID gains
//*********************************************************************************
#define YAW_KP 4.5
#define YAW_KI 0.5
#define YAW_KD 0.5

#define ALTITUDE_KP 12
#define ALTITUDE_KI 0.5
#define ALTITUDE_KD 0.5


//*********************************************************************************
// Variables to store previous error terms for PID calculation
//*********************************************************************************
static float AltitudeControl = 0;
static float YawControl = 0;


//*****************************************************************************
// PID controller for yaw control
//*****************************************************************************
float YawPIDController(float setpoint, float current_value)
{
    static float YawI = 0;
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
    YawControl = P + (YawI+dI) + D + K_COUPLING * AltitudeControl;

    if (YawControl > PWM_MAX_DUTY){
        YawControl = PWM_MAX_DUTY;
    } else if (YawControl < PWM_TAIL_MIN_DUTY) {
        YawControl = PWM_TAIL_MIN_DUTY;
    } else {
        YawI = (YawI+dI); // To prevent intergral windup
    }

    return YawControl;
}


//*****************************************************************************
// PID controller for altitude control
//*****************************************************************************
float AltitudePIDController(float setpoint, float current_value)
{
    static float AltI = 0;
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
    AltitudeControl = P + (AltI+dI) + D + PWM_HOVER_DUTY;

    if (AltitudeControl > PWM_MAX_DUTY){
        AltitudeControl = PWM_MAX_DUTY;
    } else if (AltitudeControl < PWM_MAIN_MIN_DUTY) {
        AltitudeControl = PWM_MAIN_MIN_DUTY;
    } else {
        AltI = (AltI+dI);
    }

    return AltitudeControl;
}


//*****************************************************************************
// Returns Yaw Duty Cycle
//*****************************************************************************
uint32_t getYawDuty(void)
{
    uint32_t yawDuty = YawControl;
    return yawDuty;
}


//*****************************************************************************
// Returns Altitude Duty Cycle
//*****************************************************************************
uint32_t getAltitudeDuty(void)
{
    uint32_t altitudeDuty = AltitudeControl;
    return altitudeDuty;
}
