//********************************************************************************
//
// File: display.h
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for displaying the current yaw and altitude properties
//
//*********************************************************************************

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
// Function to display the altitude as a percentage and yaw in degrees
//*****************************************************************************
void displayData(uint32_t altitude, int32_t yaw);

//*****************************************************************************
// Function to display the mean ADC value (10-bit value, note) and sample count.
//*****************************************************************************
void displayADC(uint32_t adc);

//*****************************************************************************
// Function to clear the screen
//*****************************************************************************
void cleardisplay(void);

//*****************************************************************************
// Function to initialise the OLED display
//*****************************************************************************
void initDisplay(void);

#endif /* DISPLAY_H_ */

