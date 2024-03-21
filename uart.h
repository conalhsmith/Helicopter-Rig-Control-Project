#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>

void UARTSend(char *pucBuffer);
void initialiseUSB_UART(void);

#endif /* UART_H_ */
