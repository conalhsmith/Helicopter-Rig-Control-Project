//********************************************************************************
//
// File: uart.h
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for transmission support across a serial link using UART on the Tiva board
// Uses 9600 baud, 8-bit words, 1 stop bit, no parity.
//*********************************************************************************

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>


//*********************************************************************************
// sends something
//*********************************************************************************
void UARTSend(char *pucBuffer);


//*********************************************************************************
// Initalises and configures the ports, peripherals and pins required for transmission
//*********************************************************************************
void initialiseUSB_UART(void);

#endif /* UART_H_ */
