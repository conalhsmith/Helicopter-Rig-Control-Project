//********************************************************************************
//
// File: altitude.c
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for measuring the altitude value from the circular buffer and converting
// to a percentage.
//
//*********************************************************************************


#include "altitude.h"
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "circBufT.h"
#include <stdint.h>

static circBuf_t g_inBuffer;
static int32_t helicopterLanded = 0;


//*********************************************************************************
// Constants
//*********************************************************************************
#define ADC_CONSTANT 1241  // range of ADC values in 1 volt (4095 / 3.3).
#define BUF_SIZE 32



void initAltitude(void)
{
    initCircBuf(&g_inBuffer, BUF_SIZE);
    initADC();
}

void initADC(void)
{
    //
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
                             ADC_CTL_END);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    //
    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);

    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}

void ADCIntHandler(void)
{
    uint32_t ulValue;

    //
    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
    //
    // Place it in the circular buffer (advancing write index)
    writeCircBuf (&g_inBuffer, ulValue);
    //
    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}


//*********************************************************************************
// Calculates the altitude by taking the average values stored in the buffer.
//*********************************************************************************
int32_t calculateAltitude(circBuf_t *buffer)
{

    //calculate the sum of values currently stored within the circular buffer
    int32_t sum = 0;
    uint16_t i;
    for (i = 0; i < BUF_SIZE; i++)
        sum = sum + readCircBuf(buffer);

    //calculate and return the average of values within the circular buffer
    return ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE);
}

//*********************************************************************************
// Converts the altitude to a percentage value.
//*********************************************************************************
float getAltitudePercentage()
{
    float current = helicopterLanded - calculateAltitude(&g_inBuffer);
    return (100 * current / ADC_CONSTANT);
}


void referenceAltitude(void)
{
    helicopterLanded = calculateAltitude(&g_inBuffer);
}


