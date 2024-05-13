//*****************************************************************************
//
// File: PWM.c
//
// Authors: [Your Name]
//
// Module for generating PWM signals for main rotor and tail rotor motors
//
//*****************************************************************************

#include "PWM.h"

#define PWM_FREQUENCY_MIN   150  // Minimum PWM frequency in Hz
#define PWM_FREQUENCY_MAX   300  // Maximum PWM frequency in Hz

//*****************************************************************************
// Initialize PWM modules for main rotor and tail rotor motors
//*****************************************************************************
void PWMMainInit(void)
{
    // Enable PWM peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    // Disable PWM output while configuring
    PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT, false);

    // Configure PWM options
    PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);

    // Set PWM frequency
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, SysCtlClockGet() / PWM_FREQUENCY_MAX);

    // Enable PWM output
    PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT, true);

    // Enable PWM generator
    PWMGenEnable(PWM0_BASE, PWM_GEN_3);
}

void PWMTailInit(void)
{
    // Enable PWM peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

    // Disable PWM output while configuring
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, false);

    // Configure PWM options
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);

    // Set PWM frequency
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, SysCtlClockGet() / PWM_FREQUENCY_MAX);

    // Enable PWM output
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);

    // Enable PWM generator
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
}

//*****************************************************************************
// Set duty cycle for main rotor motor
//*****************************************************************************
void PWMSetMainRotorDutyCycle(uint8_t dutyCyclePercent)
{
    // Calculate duty cycle value based on percentage
    uint32_t dutyCycleValue = PWMGenPeriodGet(PWM0_BASE, PWM_GEN_3) * dutyCyclePercent / 100;

    // Set duty cycle
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, dutyCycleValue);
}

//*****************************************************************************
// Set duty cycle for tail rotor motor
//*****************************************************************************
void PWMSetTailRotorDutyCycle(uint8_t dutyCyclePercent)
{
    // Calculate duty cycle value based on percentage
    uint32_t dutyCycleValue = PWMGenPeriodGet(PWM1_BASE, PWM_GEN_2) * dutyCyclePercent / 100;

    // Set duty cycle
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, dutyCycleValue);
}

