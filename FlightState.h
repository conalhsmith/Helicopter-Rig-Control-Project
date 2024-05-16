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

// File: state.h
#ifndef FLIGHTSTATE_H
#define FLIGHTSTATE_H

// Enum for helicopter flight states
typedef enum {
    LANDED,
    TAKING_OFF,
    HOVERING,
    LANDING
} HelicopterMode;

// Function prototypes
void setState(HelicopterMode mode);
HelicopterMode getState();

#endif  // FLIGHTSTATE_H
