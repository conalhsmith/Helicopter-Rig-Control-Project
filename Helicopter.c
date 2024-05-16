//********************************************************************************
//
// File: main.c
//
// Authors: Conal Smith
//          Adam Mason
//
//*********************************************************************************


#include <FlightState.h>
#include <inputs.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "uart.h"
#include "display.h"
#include "altitude.h"
#include "yaw.h"
#include "controller.h"
#include "PWM.h"
#include "inputs.h"
#include "kernal.h"


//*********************************************************************************
// Constants
//*********************************************************************************
#define SAMPLE_RATE_HZ 150
#define SLOWTICK_RATE_HZ 4


//*********************************************************************************
// variables for sysTick interrupt handler
//*********************************************************************************
static uint32_t g_ulSampCnt;
volatile uint8_t slowTick = false;


//*****************************************************************************
// The interrupt handler for the for SysTick interrupt.
//*****************************************************************************
void SysTickIntHandler(void) {
    ADCProcessorTrigger(ADC0_BASE, 3);
    g_ulSampCnt++;

    static uint8_t tickCount = 0;
    const uint8_t ticksPerSlow = SAMPLE_RATE_HZ / SLOWTICK_RATE_HZ;

    if (++tickCount >= ticksPerSlow) {
        tickCount = 0;
        slowTick = true;
    }
}


//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick)
//*****************************************************************************
void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}


//*****************************************************************************
// Main Function
//*****************************************************************************
int main(void) {

    // Initialize system and peripherals
    PWMMainInit();
    PWMTailInit();
    initClock();
    initAltitude();
    initDisplay();
    initialiseUSB_UART();
    initButtons();
    initSwitch();
    initYaw();

    //Initialize interrupts
    IntMasterEnable();

    while (1) {

        // Update button states
        updateButtons();
        updateSwitch();
        //call scheduler to perform tasks
        scheduler();

    }
}



