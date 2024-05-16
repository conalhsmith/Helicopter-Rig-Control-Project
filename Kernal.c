//********************************************************************************
//
// File: kernal.c
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for handling the scheduling of each task, aswell as calling other modules
// to perform the tasks.
//
//*********************************************************************************

#include <FlightState.h>
#include <stdint.h>
#include "PWM.h"
#include "altitude.h"
#include "yaw.h"
#include "controller.h"
#include "inputs.h"
#include "display.h"
#include "uart.h"


//*********************************************************************************
// constants
//*********************************************************************************
#define PWM_FREQUENCY 250
#define LANDING_ALTITUDE_THRESHOLD 1
#define ALTITUDE_STEP 10 //amount altitude changes per button press (%)
#define YAW_STEP 15 //amount yaw changes per button press (degrees)
#define TAKEOFF_DUTY 44

//*********************************************************************************
// Static varibles for both current and desired yaw and altitude
//*********************************************************************************
static float CurrentAltitudePercentage = 0;
static float currentYawDegrees = 0;
static int32_t setAltitude = 0;
static int16_t setYaw = 0;


//*********************************************************************************
// Handles the takeoff routine for the helicopter
//*********************************************************************************
void handleTakeOff(HelicopterMode *mode) {
    PWMSetTailRotorDutyCycle(PWM_FREQUENCY, TAKEOFF_DUTY);

    if (CheckReferencePin()) {
        ResetYawToZero(); //calibrate yaw to be zero at reference point
        *mode = HOVERING;
    }
}


//*********************************************************************************
// Checks button inputs and adjusts setpoint accordingly
//*********************************************************************************
void checkButtons(void){
    uint8_t up_button_state = checkButton(UP);
    uint8_t down_button_state = checkButton(DOWN);
    uint8_t ccw_button_state = checkButton(LEFT);
    uint8_t cw_button_state = checkButton(RIGHT);

    if (up_button_state == PUSHED) {
        setAltitude += ALTITUDE_STEP;
    } else if (down_button_state == PUSHED) {
        setAltitude -= ALTITUDE_STEP;
    } else if (ccw_button_state == PUSHED) {
        setYaw -= YAW_STEP;
    } else if (cw_button_state == PUSHED) {
        setYaw += YAW_STEP;
    }
}


//*********************************************************************************
// Handles the hovering routine for the helicopter
//*********************************************************************************
void handleHovering(HelicopterMode *mode, uint8_t sliderState) {
    if (sliderState == SWITCH_DOWN) {
        *mode = LANDING;
    }
    checkButtons();
    if (setAltitude > 100) {
        setAltitude = 100;
    } else if (setAltitude < 0) {
        setAltitude = 0;
    }

    int16_t altitudeEffort = AltitudePIDController(setAltitude, CurrentAltitudePercentage);
    int16_t yawEffort = YawPIDController(setYaw, currentYawDegrees);
    PWMSetMainRotorDutyCycle(PWM_FREQUENCY, altitudeEffort);
    PWMSetTailRotorDutyCycle(PWM_FREQUENCY, yawEffort);
}


//*********************************************************************************
// Handles the landing routine for the helicopter
//*********************************************************************************
void handleLanding(HelicopterMode *mode) {
    setYaw = 0;
    int16_t yawEffortLanding = YawPIDController(setYaw, currentYawDegrees);
    int16_t altitudeEffortLanding = AltitudePIDController(setAltitude, CurrentAltitudePercentage);

    PWMSetTailRotorDutyCycle(PWM_FREQUENCY, yawEffortLanding);
    PWMSetMainRotorDutyCycle(PWM_FREQUENCY, altitudeEffortLanding);

    if (CheckReferencePin()) {
        setAltitude = 0;
        currentYawDegrees = 0;
        if (CurrentAltitudePercentage <= LANDING_ALTITUDE_THRESHOLD) {
            PWMSetMainRotorDutyCycle(PWM_FREQUENCY, 0);
            PWMSetTailRotorDutyCycle(PWM_FREQUENCY, 0);
            *mode = LANDED;
        }
    }
}


//*********************************************************************************
// Schedules the state machine
//*********************************************************************************
void scheduler() {
    HelicopterMode mode = getState();
    uint8_t sliderState = checkSwitch();
    CurrentAltitudePercentage = getAltitudePercentage();
    currentYawDegrees = getYawDegrees();

    switch (mode) {
        case LANDED:
            if (sliderState == SWITCH_UP) {
                mode = TAKING_OFF;
            }
            break;

        case TAKING_OFF:
            handleTakeOff(&mode);
            break;

        case HOVERING:
            handleHovering(&mode, sliderState);
            break;

        case LANDING:
            handleLanding(&mode);
            break;
    }

    setState(mode);
    displayData();
    uartSendStatus(setAltitude, setYaw);
}




