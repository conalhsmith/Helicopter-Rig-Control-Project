#include "display.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "utils/ustdlib.h"

//*****************************************************************************
//
// Function to display the mean ADC value (10-bit value, note) and sample count.
//
//*****************************************************************************
void
displayMeanVal(uint16_t meanVal, uint32_t count)
{
    char string[17];  // 16 characters across the display

    OLEDStringDraw ("ADC demo 2", 0, 0);

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "Mean ADC = %4d", meanVal);
    // Update line on display.
    OLEDStringDraw (string, 0, 1);

    usnprintf (string, sizeof(string), "Sample # %5d", count);
    OLEDStringDraw (string, 0, 3);
}

void
displayAltitude(uint32_t altitude)
{
    char string[17];  // 16 characters across the display

    usnprintf (string, sizeof(string), "altitude: %5d%%", altitude);
    OLEDStringDraw (string, 0, 1);
}

void displayADC(uint32_t adc) {
    char string[17];
    usnprintf(string, sizeof(string), "Mean ADC: %5d      ", adc);
    OLEDStringDraw(string, 0, 1);
}

void cleardisplay(void) {
    OLEDStringDraw("                ", 0, 1); // Clear the display
}

void initDisplay(void) {
    OLEDInitialise();
}

