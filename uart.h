//********************************************************************************
//
// File: uart.h
//
// Authors: Conal Smith
//          Adam Mason
//
// Module for transmission support across a serial link using UART on the Tiva board
// Uses 9600 baud, 8-bit words, 1 stop bit, no parity.
//
// This module contains some code from the labs and lectures on learn
//
//*********************************************************************************

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>


//*********************************************************************************
// Sends a string via UART.
// Code template taken from lectures on learn
//*********************************************************************************
void UARTSend(char *pucBuffer);


//*********************************************************************************
// Initalises and configures the ports, peripherals and pins required for transmission
// Code template taken from lectures on learn
//*********************************************************************************
void initialiseUSB_UART(void);


//*****************************************************************************
// Transmits a message containing information about the status of the program.
// Code template taken from lectures on learn
//*****************************************************************************
void uartSendStatus(uint32_t, uint32_t);


#endif /* UART_H_ */
