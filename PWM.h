//*****************************************************************************
//
// File: PWM.h
//
// Authors: [Your Name]
//
// Module for generating PWM signals for main rotor and tail rotor motors
//
//*****************************************************************************

#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"

//*****************************************************************************
// Initialize PWM modules for main rotor and tail rotor motors
//*****************************************************************************
void PWMMainInit(void);

//*****************************************************************************
// Initialize PWM modules for main rotor and tail rotor motors
//*****************************************************************************
void PWMTailInit(void);

//*****************************************************************************
// Set duty cycle for main rotor motor
//*****************************************************************************
void PWMSetMainRotorDutyCycle(uint32_t, uint32_t);

//*****************************************************************************
// Set duty cycle for tail rotor motor
//*****************************************************************************
void PWMSetTailRotorDutyCycle(uint32_t, uint32_t);

#endif /* PWM_H_ */
