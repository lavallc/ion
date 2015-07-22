#include "lava_bootloader.h"
#include "nrf_soc.h"
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"

void sd_triggerBootloader(void)
{
	uint32_t boot_set = 0xFF;
	sd_power_gpregret_set(boot_set);
	sd_nvic_SystemReset();
}

void triggerBootloader(void)
{
	uint32_t boot_set = 0xFF;
	NRF_POWER->GPREGRET = boot_set;
	NVIC_SystemReset();
}

void maskBootloader(uint8_t mask)
{
	uint32_t boot_set = 0;
	boot_set = NRF_POWER->GPREGRET;
	boot_set = boot_set |(uint32_t)mask;
	NRF_POWER->GPREGRET = boot_set;
	NVIC_SystemReset();
}

void sd_maskBootloader(uint8_t mask)
{
	uint32_t boot_set = 0;
	sd_power_gpregret_get(&boot_set);
	boot_set = boot_set |(uint32_t)mask;
	sd_power_gpregret_set(boot_set);
	sd_nvic_SystemReset();
}

void clrBootloader(uint8_t mask)
{
	uint32_t boot_set = 0;
	boot_set = NRF_POWER->GPREGRET;
	boot_set = boot_set &(~(uint32_t)mask);
	NRF_POWER->GPREGRET = boot_set;
}

void sd_clrBootloader(uint8_t mask)
{
	sd_power_gpregret_clr(mask);
}

uint8_t getBootloader(void)
{
	return (uint8_t)(NRF_POWER->GPREGRET);
}

uint8_t sd_getBootloader(void)
{
	uint32_t boot_set = 0;
	sd_power_gpregret_get(&boot_set);
	return (uint8_t)boot_set;
}

