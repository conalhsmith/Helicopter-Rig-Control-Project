//********************************************************************************
//
// File: display.c
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for displaying the current yaw and altitude properties
// aswell as the  current duty cycles for both the main and tail rotors.

//*********************************************************************************

#include "display.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "utils/ustdlib.h"
#include "altitude.h"
#include "yaw.h"
#include "controller.h"
#include <stdint.h>


//*****************************************************************************
// Function to initialise the OLED display
//*****************************************************************************
void initDisplay(void) {
    OLEDInitialise();
}

//*****************************************************************************
// Function to display the altitude as a percentage and yaw in degrees
//*****************************************************************************
void displayData()
{
    char string[17];

    // Display altitude
    int32_t altitude = getAltitudePercentage();
    usnprintf(string, sizeof(string), "Altitude: %5d%%", altitude);
    OLEDStringDraw(string, 0, 1);


    // Display yaw
    int32_t yaw = getYawDegrees();
    usnprintf(string, sizeof(string), "Yaw: %d deg     ", yaw);
    OLEDStringDraw(string, 0, 2);


    // Display motor duty cycle's
    usnprintf(string, sizeof(string), "Main:%d%% Tail:%d%%", getAltitudeDuty(), getYawDuty());
    OLEDStringDraw(string, 0, 3);

}

