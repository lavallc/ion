#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"

void triggerBootloader(void);

void sd_triggerBootloader(void);

void maskBootloader(uint8_t mask);

void sd_maskBootloader(uint8_t mask);

void clrBootloader(uint8_t mask);

void sd_clrBootloader(uint8_t mask);

uint8_t getBootloader(void);

uint8_t sd_getBootloader(void);
