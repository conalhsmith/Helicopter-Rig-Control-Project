//*****************************************************************************
//
// ADCdemo1.c - Simple interrupt driven program which samples with AIN0
//
// Author:  P.J. Bones	UCECE
// Last modified:	8.2.2018
//
//*****************************************************************************
// Based on the 'convert' series from 2016
//*****************************************************************************

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

//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE 32
#define SAMPLE_RATE_HZ 150
#define SLOWTICK_RATE_HZ 4

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t g_inBuffer;		// Buffer of size BUF_SIZE integers (sample values)
static uint32_t g_ulSampCnt;	// Counter for the interrupts
volatile uint8_t slowTick = false;
char statusStr[1]; //??

//*****************************************************************************
// USB Serial comms: UART0, Rx:PA0 , Tx:PA1
//*****************************************************************************
#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX


//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    //
    // Initiate a conversion
    //
    ADCProcessorTrigger(ADC0_BASE, 3); 
    g_ulSampCnt++;

    static uint8_t tickCount = 0;
    const uint8_t ticksPerSlow = SAMPLE_RATE_HZ / SLOWTICK_RATE_HZ;

    if (++tickCount >= ticksPerSlow)
    {                       // Signal a slow tick
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

void
initialiseUSB_UART (void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    //
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);
    //
    // Select the alternate (UART) function for these pins.
    //
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);

    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);
}

void
initDisplay (void)
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}

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

void
displayADC(uint32_t adc)
{
    char altitudeString[17];
    usnprintf(altitudeString, sizeof(altitudeString), "mean ADC: %5d   ", adc);
    OLEDStringDraw(altitudeString, 0, 1);
}

void
cleardisplay()
{
    OLEDStringDraw("                          ", 0, 1); // Clear the display
}
//*****************************************************************************
// Transmit a string via UART0
//*****************************************************************************
void
UARTSend (char *pucBuffer)
{
    // Loop while there are more characters to send.
    while(*pucBuffer)
    {
        // Write the next character to the UART Tx FIFO.
        UARTCharPut(UART_USB_BASE, *pucBuffer);
        pucBuffer++;
    }
}

// Background task: calculate the (approximate) mean of the values in the
// circular buffer and display it, together with the sample number.
int32_t
calculateAltitude()
{
    int32_t sum = 0;
    uint16_t i;
    for (i = 0; i < BUF_SIZE; i++)
        sum = sum + readCircBuf (&g_inBuffer);

    return ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE);

}

int32_t
altitudepercentage(int32_t currentAltitude)
{
    return (((int32_t)100 * currentAltitude) / ((int32_t)1241));
}


int main(void)
{
    uint16_t i;
    int32_t sum = 0;
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

    // Enable interrupts to the processor.
    IntMasterEnable();

    while (1)
    {
        // Calculate the current altitude
        updateButtons();

        if (!zero_ref)
        {
            helicopterLanded = calculateAltitude();
            zero_ref = true;
        }

        currentAltitude = helicopterLanded - calculateAltitude();

        uint8_t up_but_state = checkButton(UP);
        uint8_t left_but_state = checkButton(LEFT);

        switch (up_but_state)
        {
            case PUSHED:
                state++;
                if (state == 3)
                {
                    state = 0;
                }
                break;
            case RELEASED:
            case NO_CHANGE:
            default:
                break;
        }

        switch (state)
        {
            case 0:
                // Display percentage altitude
                break;
            case 1:
                // Display mean ADC value
                break;
            case 2:
                // Turn off display
                cleardisplay();
                break;
            default:
                break;
        }

        if (left_but_state == PUSHED)
        {
            zero_ref = false;
        }


        if (state == 0)
        {

            int32_t altitudePercentage = altitudepercentage(currentAltitude);

            // Display the altitude value on the Orbit OLED display

             displayAltitude(altitudePercentage);
        }

        if (state == 1)
        {
            int32_t meanADC = calculateAltitude();
            displayADC(meanADC);

        }


        SysCtlDelay(SysCtlClockGet() / 30);  // Update display at ~ 2 Hz
    }
}





