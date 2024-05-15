//*****************************************************************************
//
// File: PWM.c
//
// Authors: [Your Name]
//
// Module for generating PWM signals for main rotor and tail rotor motors
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"


#define PWM_FREQUENCY_MIN   150  // Minimum PWM frequency in Hz
#define PWM_FREQUENCY_MAX   300  // Maximum PWM frequency in Hz
#define PWM_DIVIDER 4 // PWM divider

//*****************************************************************************
// Initialize PWM modules for main rotor and tail rotor motors
//*****************************************************************************
void PWMMainInit(void)
{
    // Enable PWM peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinConfigure(GPIO_PC5_M0PWM7);
    GPIOPinTypePWM(GPIO_PORTC_BASE, GPIO_PIN_5);

    PWMGenConfigure(PWM0_BASE, PWM_GEN_3,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenEnable(PWM0_BASE, PWM_GEN_3);

    PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT, true);
}


void PWMTailInit(void)
{
    // Enable PWM peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinConfigure(GPIO_PF1_M1PWM5);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);

    PWMGenConfigure(PWM1_BASE, PWM_GEN_2,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenEnable(PWM1_BASE, PWM_GEN_2);

    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
}


//*****************************************************************************
// Set duty cycle for main rotor motor
//*****************************************************************************
void PWMSetMainRotorDutyCycle(uint32_t ui32Freq, uint32_t ui32Duty)
{

    uint32_t ui32Period = SysCtlClockGet() / PWM_DIVIDER / ui32Freq;

    // Calculate duty cycle value based on percentage
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, ui32Period);

    // Set duty cycle
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, ui32Period * ui32Duty / 100);
}

//*****************************************************************************
// Set duty cycle for tail rotor motor
//*****************************************************************************
void PWMSetTailRotorDutyCycle(uint32_t ui32Freq, uint32_t ui32Duty)
{
    uint32_t ui32Period = SysCtlClockGet() / PWM_DIVIDER / ui32Freq;

    // Calculate duty cycle value based on percentage
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Period);

    // Set duty cycle
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui32Period * ui32Duty / 100);
}

