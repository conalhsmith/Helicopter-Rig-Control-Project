//********************************************************************************
//
// File: display.c
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for displaying the current yaw and altitude properties
//
//*********************************************************************************

#include "display.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "utils/ustdlib.h"
#include <stdint.h>

//*********************************************************************************
// Constants
//*********************************************************************************



//*****************************************************************************
// Function to display the altitude as a percentage and yaw in degrees
//*****************************************************************************
void displayData(uint32_t altitude, int32_t yaw)
{
    char string[17];  // 16 characters across the display

    int32_t yawInt = yaw;

    // Display altitude
    usnprintf(string, sizeof(string), "Altitude: %5d%%", altitude);
    OLEDStringDraw(string, 0, 1);

    // Display yaw
    usnprintf(string, sizeof(string), "Yaw: %d deg     ", yawInt);
    OLEDStringDraw(string, 0, 3);
}

//*****************************************************************************
// Function to display the mean ADC value (10-bit value, note) and sample count.
//*****************************************************************************
void displayADC(uint32_t adc) {
    char string[17];
    usnprintf(string, sizeof(string), "Mean ADC: %5d      ", adc);
    OLEDStringDraw(string, 0, 1);
    OLEDStringDraw("                ", 0, 3);
}

//*****************************************************************************
// Function to clear the screen
//*****************************************************************************
void cleardisplay(void) {
    OLEDStringDraw("                ", 0, 1);
    OLEDStringDraw("                ", 0, 3);
}

//*****************************************************************************
// Function to initialise the OLED display
//*****************************************************************************
void initDisplay(void) {
    OLEDInitialise();
}

