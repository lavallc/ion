
 #ifndef MPR121_H
 #define MPR121_H

#include "nrf_gpio.h"
#include <stdbool.h>

// MPR121 Register Defines
#define MPR121_TOUCH_0_7			0x00
#define MPR121_TOUCH_8_11			0x01
#define MPR121_OOR_0_7				0x02
#define MPR121_OOR_8_11				0x03
#define MPR121_FILT_LSB_0			0x04
#define MPR121_FILT_MSB_0 		0x05
#define MPR121_FILT_LSB_1			0x06
#define MPR121_FILT_MSB_1 		0x07
#define MPR121_FILT_LSB_2			0x08
#define MPR121_FILT_MSB_2 		0x09
#define MPR121_FILT_LSB_3			0x0A
#define MPR121_FILT_MSB_3 		0x0B
#define MPR121_FILT_LSB_4			0x0C
#define MPR121_FILT_MSB_4 		0x0D
#define MPR121_FILT_LSB_5			0x0E
#define MPR121_FILT_MSB_5 		0x0F
#define MPR121_FILT_LSB_6			0x10
#define MPR121_FILT_MSB_6 		0x11
#define MPR121_FILT_LSB_7			0x12
#define MPR121_FILT_MSB_7 		0x13
#define MPR121_FILT_LSB_8			0x14
#define MPR121_FILT_MSB_8 		0x15
#define MPR121_FILT_LSB_9			0x16
#define MPR121_FILT_MSB_9 		0x17
#define MPR121_FILT_LSB_10		0x18
#define MPR121_FILT_MSB_10		0x19
#define MPR121_FILT_LSB_11		0x1A
#define MPR121_FILT_MSB_11 		0x1B
#define MPR121_FILT_LSB_PROX	0x1C
#define MPR121_FILT_MSB_PROX 	0x1D
#define MPR121_BASELINE_0			0x1E
#define MPR121_BASELINE_1			0x1F
#define MPR121_BASELINE_2			0x20
#define MPR121_BASELINE_3			0x21
#define MPR121_BASELINE_4			0x22
#define MPR121_BASELINE_5			0x23
#define MPR121_BASELINE_6			0x24
#define MPR121_BASELINE_7			0x25
#define MPR121_BASELINE_8			0x26
#define MPR121_BASELINE_9			0x27
#define MPR121_BASELINE_10		0x28
#define MPR121_BASELINE_11		0x29
#define MPR121_BASELINE_PROX	0x2A
#define MPR121_MHD_R					0x2B
#define MPR121_NHD_R					0x2C
#define	MPR121_NCL_R 					0x2D
#define	MPR121_FDL_R					0x2E
#define	MPR121_MHD_F					0x2F
#define	MPR121_NHD_F					0x30
#define	MPR121_NCL_F					0x31
#define	MPR121_FDL_F					0x32
#define MPR121_NHD_T					0x33
#define MPR121_NCL_T					0x34
#define MPR121_FDL_T					0x35
#define MPR121_PROX_MHD_R			0x36
#define MPR121_PROX_NHD_R			0x37
#define	MPR121_PROX_NCL_R 		0x38
#define	MPR121_PROX_FDL_R			0x39
#define	MPR121_PROX_MHD_F			0x3A
#define	MPR121_PROX_NHD_F			0x3B
#define	MPR121_PROX_NCL_F			0x3C
#define	MPR121_PROX_FDL_F			0x3D
#define MPR121_PROX_NHD_T			0x3E
#define MPR121_PROX_NCL_T			0x3F
#define MPR121_PROX_FDL_T			0x40
#define	MPR121_ELE0_T					0x41
#define	MPR121_ELE0_R					0x42
#define	MPR121_ELE1_T					0x43
#define	MPR121_ELE1_R					0x44
#define	MPR121_ELE2_T					0x45
#define	MPR121_ELE2_R					0x46
#define	MPR121_ELE3_T					0x47
#define	MPR121_ELE3_R					0x48
#define	MPR121_ELE4_T					0x49
#define	MPR121_ELE4_R					0x4A
#define	MPR121_ELE5_T					0x4B
#define	MPR121_ELE5_R					0x4C
#define	MPR121_ELE6_T					0x4D
#define	MPR121_ELE6_R					0x4E
#define	MPR121_ELE7_T					0x4F
#define	MPR121_ELE7_R					0x50
#define	MPR121_ELE8_T					0x51
#define	MPR121_ELE8_R					0x52
#define	MPR121_ELE9_T					0x53
#define	MPR121_ELE9_R					0x54
#define	MPR121_ELE10_T				0x55
#define	MPR121_ELE10_R				0x56
#define	MPR121_ELE11_T				0x57
#define	MPR121_ELE11_R				0x58
#define MPR121_PROX_T					0x59
#define MPR121_PROX_R					0x5A
#define MPR121_DEBOUNCE				0x5B
#define MPR121_FIL_CDC				0x5C
#define	MPR121_FIL_CFG				0x5D
#define	MPR121_ELE_CFG				0x5E
#define MPR121_ELE0_C					0x5F
#define MPR121_ELE1_C					0x60
#define MPR121_ELE2_C					0x61
#define MPR121_ELE3_C					0x62
#define MPR121_ELE4_C					0x63
#define MPR121_ELE5_C					0x64
#define MPR121_ELE6_C					0x65
#define MPR121_ELE7_C					0x66
#define MPR121_ELE8_C					0x67
#define MPR121_ELE9_C					0x68
#define MPR121_ELE10_C				0x69
#define MPR121_ELE11_C				0x6A
#define MPR121_PROX_C					0x6B
#define MPR121_CHGTIME_0_1		0x6C
#define MPR121_CHGTIME_2_3		0x6D
#define MPR121_CHGTIME_4_5		0x6E
#define MPR121_CHGTIME_6_7		0x6F
#define MPR121_CHGTIME_8_9		0x70
#define MPR121_CHGTIME_10_11	0x71
#define MPR121_CHGTIME_PROX		0x72
#define MPR121_GPIO_CTRL0			0x73
#define	MPR121_GPIO_CTRL1			0x74
#define MPR121_GPIO_DATA			0x75
#define	MPR121_GPIO_DIR				0x76
#define	MPR121_GPIO_EN				0x77
#define	MPR121_GPIO_SET				0x78
#define	MPR121_GPIO_CLEAR			0x79
#define	MPR121_GPIO_TOGGLE		0x7A
#define	MPR121_AUTO_CFG0			0x7B
#define	MPR121_AUTO_CFGU			0x7D
#define	MPR121_AUTO_CFGL			0x7E
#define	MPR121_AUTO_CFGT			0x7F
#define	MPR121_SOFT_RESET			0x80


// Global Constants
#define MPR121_FIL_CFG_VAL		0x24
#define MPR121_ADDRESS_W			0xB4
#define MPR121_ADDRESS_R			0xB5
#define TOU_THRESH	0x0F
#define	REL_THRESH	0x0A

uint8_t mpr121_write_register(uint8_t address, uint8_t datain);
uint8_t mpr121_read_register(uint8_t address, uint8_t * data);
bool mpr121_exists_on_POR(void);
bool mpr121_isTouched_electrode(uint8_t electrode_num);
uint8_t mpr121_read_baseline(uint8_t electrode_num, uint16_t * baseline);
uint8_t mpr121_read_electrode(uint8_t electrode_num, uint16_t * filtered_data);
void mpr121_readAll_electrode(uint16_t * filtered_data);
void mpr121_config_quick(void);
void mpr121_config_prox_13(void);
void mpr121_config_prox_touch_0(void);

#endif