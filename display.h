#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

void displayMeanVal(uint16_t meanVal, uint32_t count);
void displayAltitude(uint32_t altitude);
void displayADC(uint32_t adc);
void cleardisplay(void);
void initDisplay(void);

#endif /* DISPLAY_H_ */

