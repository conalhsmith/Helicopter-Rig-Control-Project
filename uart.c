//********************************************************************************
//
// File: uart.c
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for transmission support across a serial link using UART on the Tiva board
// Uses 9600 baud, 8-bit words, 1 stop bit, no parity.
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

//*********************************************************************************
// Constants
//*********************************************************************************
#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX
#define STR_LEN             18


//*********************************************************************************
// Initalises and configures the ports, peripherals and pins required for transmission
//*********************************************************************************
void initialiseUSB_UART(void)
{
    // Enable GPIO port and UART peripheral
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);

    // Configure GPIO pins for UART
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    // Configure UART settings
    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
                        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                        UART_CONFIG_PAR_NONE);

    // Enable UART
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);
}

//*********************************************************************************
// sends something
//*********************************************************************************
void UARTSend(char *pucBuffer)
{
    while (*pucBuffer)
    {
        UARTCharPut(UART_USB_BASE, *pucBuffer);
        pucBuffer++;
    }
}

//*****************************************************************************
// Transmits a message containing information about the status of the program.
//*****************************************************************************
void uartSendStatus(uint32_t altitudepercentage, uint32_t altitudedesired, uint32_t yawpercentage, uint32_t yawdesired) {
    char line[STR_LEN + 1];

    usnprintf(line, sizeof(line),
              "Alt: %4d [%4d]\r\n", altitudepercentage, altitudedesired);
    UARTSend(line);

    usnprintf(line, sizeof(line),
              "Yaw: %4d [%4d]\r\n", yawpercentage, yawdesired);
    UARTSend(line);
}
