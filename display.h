//********************************************************************************
//
// File: display.h
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for displaying the current yaw and altitude properties
// aswell as the  current duty cycles for both the main and tail rotors.

//*********************************************************************************

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>


//*****************************************************************************
// Function to display the altitude as a percentage and yaw in degrees
//*****************************************************************************
void displayData();


//*****************************************************************************
// Function to initialise the OLED display
//*****************************************************************************
void initDisplay(void);

#endif /* DISPLAY_H_ */

