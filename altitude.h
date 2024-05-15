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
//*********************************************************************************


#ifndef ALTITUDE_H_
#define ALTITUDE_H_

#include <stdint.h>
#include <stdbool.h>
#include "circBufT.h"

//*********************************************************************************
// Calculates the altitude by taking the average values stored in the buffer.
//*********************************************************************************
int32_t calculateAltitude(circBuf_t *buffer);

//*********************************************************************************
// Converts the altitude to a percentage value.
//*********************************************************************************
float getAltitudePercentage();


void initAltitude(void);

void initADC(void);
void ADCIntHandler(void);
void referenceAltitude(void);


#endif /* ALTITUDE_H_ */
