#include "altitude.h"
#include "circBufT.h"

int32_t calculateAltitude(circBuf_t *buffer, uint16_t BUF_SIZE) {
    int32_t sum = 0;
    uint16_t i;
    for (i = 0; i < BUF_SIZE; i++)
        sum = sum + readCircBuf(buffer);

    return ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE);
}

int32_t altitudepercentage(int32_t currentAltitude) {
    return (100 * currentAltitude / 1241);
}
