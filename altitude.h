#ifndef ALTITUDE_H_
#define ALTITUDE_H_

#include <stdint.h>
#include "circBufT.h"

int32_t calculateAltitude(circBuf_t *buffer, uint16_t BUF_SIZE);
int32_t altitudepercentage(int32_t currentAltitude);

#endif /* ALTITUDE_H_ */
