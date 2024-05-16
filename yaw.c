//********************************************************************************
//
// File: yaw.c
//
// Authors: Conal Smith
//          Adam Mason
//
// Module to handle finite state machine and interupt handler for yaw and convert to degrees.
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
#define NUM_SLOTS 112 //Number of slots on helicopter encoder
#define DEGREES_CIRCLE 360
#define YAW_PER_SLOT 4 //yaw per slot
#define MAX_YAW 224 //corresponds to 180 degrees
#define YAW_REFERENCE_PIN_C GPIO_PIN_4
#define YAW_REFERENCE_PORT_BASE GPIO_PORTC_BASE


//*********************************************************************************
// Volatile variables for the current yaw and degrees
//*********************************************************************************
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


    //set previous channel state as current channel state
    ChannelAPrev = ChannelAActive;
    ChannelBPrev = ChannelBActive;

    //clear the Yaw pin readings
    GPIOIntClear(YAW_PORT_BASE, YAW_PIN_A | YAW_PIN_B);
}


//*********************************************************************************
// Returns true if the helicopter is on the reference pin
//*********************************************************************************
bool CheckReferencePin(void)
{
    return GPIOPinRead(GPIO_PORTC_BASE, YAW_REFERENCE_PIN_C) == 0;
}


//*********************************************************************************
// Inititiates the interupt and required ports for then yaw module.
//*********************************************************************************
void initYaw(void)
{
    SysCtlPeripheralEnable(YAW_PERIPH);
    GPIOPinTypeGPIOInput(YAW_PORT_BASE, YAW_PIN_A | YAW_PIN_B);
    GPIOPadConfigSet(YAW_PORT_BASE, YAW_PIN_A | YAW_PIN_B, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOInput(YAW_REFERENCE_PORT_BASE, YAW_REFERENCE_PIN_C);


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


//*********************************************************************************
// resets yaw to 0
//*********************************************************************************
void ResetYawToZero(void)
{
    yaw = 0;
}
