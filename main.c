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
#include "buttons.h"
#include "uart.h"
#include "display.h"
#include "altitude.h"

#define BUF_SIZE 32
#define SAMPLE_RATE_HZ 150
#define SLOWTICK_RATE_HZ 4

static circBuf_t g_inBuffer;
static uint32_t g_ulSampCnt;
volatile uint8_t slowTick = false;


//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
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
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void
ADCIntHandler(void)
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

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display, UART
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

void
initADC (void)
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

int main(void) {
    int32_t helicopterLanded;
    int32_t currentAltitude = 0;
    uint8_t state = 0;

    initClock();
    initADC();
    initDisplay();
    initCircBuf(&g_inBuffer, BUF_SIZE);
    initialiseUSB_UART();
    initButtons();
    bool zero_ref = false;

    SysCtlDelay(SysCtlClockGet() / 6);
    IntMasterEnable();

    while (1) {
        updateButtons();

        if (!zero_ref) {
            helicopterLanded = calculateAltitude(&g_inBuffer, BUF_SIZE);
            zero_ref = true;
        }

        currentAltitude = helicopterLanded - calculateAltitude(&g_inBuffer, BUF_SIZE);

        uint8_t up_but_state = checkButton(UP);
        uint8_t left_but_state = checkButton(LEFT);

        switch (up_but_state) {
            case PUSHED:
                state++;
                if (state == 3) {
                    state = 0;
                }
                break;
            case RELEASED:
            case NO_CHANGE:
            default:
                break;
        }

        switch (state) {
            case 0:
                break;
            case 1:
                break;
            case 2:
                cleardisplay();
                break;
            default:
                break;
        }

        if (left_but_state == PUSHED) {
            zero_ref = false;
        }

        if (state == 0) {
            int32_t altitudePercentage = altitudepercentage(currentAltitude);
            displayAltitude(altitudePercentage);
        }

        if (state == 1) {
            int32_t meanADC = calculateAltitude(&g_inBuffer, BUF_SIZE);
            displayADC(meanADC);
        }

        SysCtlDelay(SysCtlClockGet() / 30);
    }
}



