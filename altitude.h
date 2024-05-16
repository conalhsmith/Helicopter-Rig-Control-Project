//********************************************************************************
//
// File: altitude.h
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for measuring the altitude value from the circular buffer and converting
// to a percentage.
//
// This module contains some code from the labs and lectures on learn
//
//*********************************************************************************


#ifndef ALTITUDE_H_
#define ALTITUDE_H_

#include <stdint.h>
#include <stdbool.h>
#include "circBufT.h"

//*********************************************************************************
// Calculates the altitude by taking the average values stored in the buffer.
//*********************************************************************************
int32_t calculateAltitude(circBuf_t *);


//*********************************************************************************
// Converts the altitude to a percentage value.
//*********************************************************************************
float getAltitudePercentage();


//*********************************************************************************
// initalises functions for altitude
//*********************************************************************************
void initAltitude(void);


//*********************************************************************************
// initalises ADC
// Code template taken from lectures on learn
//*********************************************************************************
void initADC(void);


//*****************************************************************************
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
// Code template taken from lectures on learn
//*****************************************************************************
void ADCIntHandler(void);


//*********************************************************************************
// Finds ADC value at start up and sets this too landed reference
//*********************************************************************************
void referenceAltitude(void);


#endif /* ALTITUDE_H_ */
