//********************************************************************************
//
// File: main.c
//
// Authors: Conal Smith
//          Adam Mason
//
//*********************************************************************************


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


//*********************************************************************************
// Constants
//*********************************************************************************
#define SAMPLE_RATE_HZ 150
#define SLOWTICK_RATE_HZ 4
#define PWM_FREQUENCY 250

static uint32_t g_ulSampCnt;
volatile uint8_t slowTick = false;

//*********************************************************************************
// Define states for helicopter mode
//*********************************************************************************

typedef enum {
    LANDED,
    TAKING_OFF,
    HOVERING,
    LANDING
} HelicopterMode;

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
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//*****************************************************************************


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

    float CurrentAltitudePercentage = 0;
    float currentYawDegrees = 0;

    int32_t setAltitude = 0;
    int16_t setYaw = 0;

    int32_t takeoffAltitudeThreshold = 1;
    int32_t landingAltitudeThreshold = 1;

    HelicopterMode mode = LANDED;
    SysCtlDelay(SysCtlClockGet() / 6);
    referenceAltitude();
    IntMasterEnable();

    while (1) {

        // Update button states
        updateButtons();
        updateSwitch();


        // Check for slider switch state
        uint8_t sliderState = checkSwitch();


        CurrentAltitudePercentage = getAltitudePercentage();
        currentYawDegrees = getYawDegrees();

        // Handle helicopter mode based on slider switch state

        switch (mode) {

            case LANDED:

                if (sliderState == SWITCH_UP) {
                    mode = TAKING_OFF;
                }
                break;

            case TAKING_OFF:
                PWMSetMainRotorDutyCycle(PWM_FREQUENCY, 60);
                if (CurrentAltitudePercentage >= takeoffAltitudeThreshold) {
                    mode = HOVERING;
                }
                break;

            case HOVERING:
                if (sliderState == SWITCH_DOWN) {
                    mode = LANDING;

                }

                uint8_t up_button_state = checkButton(UP);
                uint8_t down_button_state = checkButton(DOWN);
                uint8_t ccw_button_state = checkButton(LEFT);
                uint8_t cw_button_state = checkButton(RIGHT);

                if (up_button_state == PUSHED) {
                    setAltitude += 10;
                } else if (down_button_state == PUSHED) {
                    setAltitude -= 10;
                } else if (ccw_button_state == PUSHED) {
                    setYaw -= 15;
                } else if (cw_button_state == PUSHED) {
                    setYaw += 15;
                }

                if (setAltitude > 100){
                    setAltitude = 100;
                } else if (setAltitude < 0) {
                    setAltitude = 0;
                }

                // Calculate PWM effort using altitude and yaw setpoints
                int16_t altitudeEffort = AltitudePIDController(setAltitude, CurrentAltitudePercentage);
                int16_t yawEffort = YawPIDController(setYaw, currentYawDegrees);

                // Apply PWM effort to rotor motors
                PWMSetMainRotorDutyCycle(PWM_FREQUENCY, altitudeEffort);
                PWMSetTailRotorDutyCycle(PWM_FREQUENCY, yawEffort);
                break;

            case LANDING:

                setAltitude = 0;

                int16_t yawEffortLanding = YawPIDController(setYaw, currentYawDegrees);
                PWMSetTailRotorDutyCycle(PWM_FREQUENCY, yawEffortLanding);
                PWMSetMainRotorDutyCycle(PWM_FREQUENCY, 40);

                if (CurrentAltitudePercentage <= landingAltitudeThreshold) {
                    PWMSetMainRotorDutyCycle(PWM_FREQUENCY, 0);
                    PWMSetTailRotorDutyCycle(PWM_FREQUENCY, 0);
                    mode = LANDED;
                }
                break;
            default:
                break;
        }

        displayData();

        uartSendStatus(setAltitude,setYaw);

    }
}



