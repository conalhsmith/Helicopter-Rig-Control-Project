//********************************************************************************
//
// File: yaw.c
//
// Authors: Conal Smith
//          Adam Mason
//
// Module to handle finite state machine for yaw and convert to degrees.
//
//*********************************************************************************


#include "yaw.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"

//*********************************************************************************
// Constants
//*********************************************************************************
#define YAW_PIN_A GPIO_PIN_0
#define YAW_PIN_B GPIO_PIN_1
#define YAW_PORT_BASE GPIO_PORTB_BASE
#define YAW_PERIPH SYSCTL_PERIPH_GPIOB
#define NUM_SLOTS 112 //Number of slots
#define DEGREES_CIRCLE 360
#define YAW_PER_SLOT 4 //yaw per slot
#define MAX_YAW 224 //corresponds to 180 degrees

volatile int32_t yaw = 0;
volatile int32_t degrees = 0;


//*********************************************************************************
// Finite state machine for encoder position.
//*********************************************************************************
void YawIntHandler(void)
{
    //initialise previous channels
    static bool ChannelAPrev = false;
    static bool ChannelBPrev = false;

    //get current state of channels
    bool ChannelAActive = GPIOPinRead(GPIO_PORTB_BASE, YAW_PIN_A) == YAW_PIN_A;
    bool ChannelBActive = GPIOPinRead(GPIO_PORTB_BASE, YAW_PIN_B) == YAW_PIN_B;

    //change yaw value depending on channel states
    if (!ChannelAPrev &&  !ChannelBPrev) {
        if (!ChannelAActive && ChannelBActive) {
            yaw++;
        } else if (ChannelAActive && !ChannelBActive) {
            yaw--;
        }
    } else if (!ChannelAPrev && ChannelBPrev) {
        if (ChannelAActive && ChannelBActive) {
            yaw++;
        } else if (!ChannelAActive && !ChannelBActive) {
            yaw--;
        }
    } else if (ChannelAPrev && !ChannelBPrev) {
        if (!ChannelAActive && !ChannelBActive) {
            yaw++;
        } else if (ChannelAActive && ChannelBActive) {
            yaw--;
        }
    } else {
        if (ChannelAActive && !ChannelBActive) {
            yaw++;
        } else if (!ChannelAActive && ChannelBActive) {
            yaw--;
        }
    }

    //check if yaw reset needed
    //ResetYaw();

    //set previous channel state as current channel state
    ChannelAPrev = ChannelAActive;
    ChannelBPrev = ChannelBActive;

    //clear the......
    GPIOIntClear(YAW_PORT_BASE, YAW_PIN_A | YAW_PIN_B);
}


//*********************************************************************************
// Resets yaw value based if it reaches max yaw in either direction.
//*********************************************************************************
void ResetYaw(void)
{
    if (yaw > (MAX_YAW)) {
            yaw = -MAX_YAW;
        } else if (yaw < -(MAX_YAW)) {
            yaw = MAX_YAW;
        }
}

//*********************************************************************************
// Inititiates the interupt and required ports for then yaw module.
//*********************************************************************************
void initYaw(void)
{
    SysCtlPeripheralEnable(YAW_PERIPH);
    GPIOPinTypeGPIOInput(YAW_PORT_BASE, YAW_PIN_A | YAW_PIN_B);
    GPIOPadConfigSet(YAW_PORT_BASE, YAW_PIN_A | YAW_PIN_B, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntDisable(YAW_PORT_BASE, YAW_PIN_A | YAW_PIN_B);
    GPIOIntClear(YAW_PORT_BASE, YAW_PIN_A | YAW_PIN_B);
    GPIOIntRegister(YAW_PORT_BASE, YawIntHandler);
    GPIOIntTypeSet(YAW_PORT_BASE, YAW_PIN_A | YAW_PIN_B, GPIO_BOTH_EDGES);
    GPIOIntEnable(YAW_PORT_BASE, YAW_PIN_A | YAW_PIN_B);
}


//*********************************************************************************
// Converts the yaw value into degrees
//*********************************************************************************
float getYawDegrees(void)
{
    return ((yaw * DEGREES_CIRCLE) / NUM_SLOTS / YAW_PER_SLOT);
}
