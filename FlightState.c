//********************************************************************************
//
// File: FlightState.c
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for tracking and changing the current flight state of helicopter
//
//*********************************************************************************

// File: state.c
#include <FlightState.h>

// Global variable to store the current state
static HelicopterMode currentMode = LANDED;

// Function to set the current state
void setState(HelicopterMode mode) {
    currentMode = mode;
}

// Function to get the current state
HelicopterMode getState() {
    return currentMode;
}
