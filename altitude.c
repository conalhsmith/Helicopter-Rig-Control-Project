//********************************************************************************
//
// File: altitude.c
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for measuring the altitude value from the circular buffer and converting
// to a percentage.
//
//*********************************************************************************


#include "altitude.h"
#include "circBufT.h"
#include <stdint.h>

//*********************************************************************************
// Constants
//*********************************************************************************
#define ADC_CONSTANT 1241  // range of ADC values in 1 volt (4095 / 3.3).

//*********************************************************************************
// Calculates the altitude by taking the average values stored in the buffer.
//*********************************************************************************
int32_t calculateAltitude(circBuf_t *buffer, uint16_t BUF_SIZE) {

    //calculate the sum of values currently stored within the circular buffer
    int32_t sum = 0;
    uint16_t i;
    for (i = 0; i < BUF_SIZE; i++)
        sum = sum + readCircBuf(buffer);

    //calculate and return the average of values within the circular buffer
    return ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE);
}

//*********************************************************************************
// Converts the altitude to a percentage value.
//*********************************************************************************
int32_t altitudepercentage(int32_t currentAltitude) {
    return (100 * currentAltitude / ADC_CONSTANT);
}
