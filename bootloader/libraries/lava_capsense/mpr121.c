#include "mpr121.h"
#include "twi_master.h"


uint8_t mpr121_write_register(uint8_t address, uint8_t datain)
{
	if (address > 0x80)
		return 1;
	uint8_t data[2];
	data[0] = address;
	data[1] = datain;
	twi_master_transfer(MPR121_ADDRESS_W, data, 2, true);
	return 0;
}

uint8_t mpr121_read_register(uint8_t address, uint8_t * data)
{
	if (address > 0x80)
		return 1;
	*data = address;
	twi_master_transfer(MPR121_ADDRESS_W, data, 1, false);
	twi_master_transfer(MPR121_ADDRESS_R, data, 1, true);
	
	return 0;
}

bool mpr121_exists_on_POR(void)
{
	uint8_t data = MPR121_FIL_CFG;
	twi_master_transfer(MPR121_ADDRESS_W, &data, 1, false);
	twi_master_transfer(MPR121_ADDRESS_R, &data, 1, true);
	if (data == MPR121_FIL_CFG_VAL)
		return true;
	
	return false;
}

uint8_t mpr121_read_electrode(uint8_t electrode_num, uint16_t * filtered_data)
{
	if (electrode_num > 11)
		return 1;
	uint8_t data[2];
	data[0] = MPR121_FILT_LSB_0 + (2 * electrode_num);
	twi_master_transfer(MPR121_ADDRESS_W, &data[0], 1, false);
	twi_master_transfer(MPR121_ADDRESS_R, data, 2, true);
	*filtered_data = (((uint16_t) (0x03U & data[1])) << 8) | data[0];
	return 0;
}

bool mpr121_isTouched_electrode(uint8_t electrode_num)
{
	if (electrode_num > 11)
		return false;
	
	uint8_t data;
	if (electrode_num  < 8)
		data = MPR121_TOUCH_0_7;
	else
		data = MPR121_TOUCH_8_11;
	twi_master_transfer(MPR121_ADDRESS_W, &data, 1, false);
	twi_master_transfer(MPR121_ADDRESS_R, &data, 1, true);
	if (electrode_num  < 8)
	{
		if ((data & (1<<electrode_num)) > 0)
			return true;
	}
	else
		{
		if ((data & (1<<(electrode_num-8))) > 0)
			return true;
	}
	
	return false;
}

uint8_t mpr121_read_baseline(uint8_t electrode_num, uint16_t * baseline)
{
	if (electrode_num > 11)
		return 1;
	
	uint8_t data;
	data = MPR121_BASELINE_0 + electrode_num;
	twi_master_transfer(MPR121_ADDRESS_W, &data, 1, false);
	twi_master_transfer(MPR121_ADDRESS_R, &data, 1, true);
	*baseline = (uint16_t)data << 2;
	return 0;
}

void mpr121_readAll_electrode(uint16_t * filtered_data)
{
	uint8_t data[26];
	
	data[0] = MPR121_FILT_LSB_0;
	twi_master_transfer(MPR121_ADDRESS_W, &data[0], 1, false);
	twi_master_transfer(MPR121_ADDRESS_R, data, 26, true);
	for (int i = 0; i < 26; i = i + 2)
	{
		filtered_data[i/2] = (((uint16_t) (0x03U & data[i+1])) << 8) | (uint16_t)data[i];
	}
}

void mpr121_config_quick(void)
{
	// Section A
	// This group controls filtering when data is > baseline.
	mpr121_write_register(MPR121_MHD_R, 0x01);
	mpr121_write_register(MPR121_NHD_R, 0x01);
	mpr121_write_register(MPR121_NCL_R, 0x00);
	mpr121_write_register(MPR121_FDL_R, 0x00);
	
	// Section B
	// This group controls filtering when data is < baseline.
	mpr121_write_register(MPR121_MHD_F, 0x01);
	mpr121_write_register(MPR121_NHD_F, 0x01);
	mpr121_write_register(MPR121_NCL_F, 0xFF);
	mpr121_write_register(MPR121_FDL_F, 0x02);
	
	// Section C
	// This group sets touch and release thresholds for each electrode
	mpr121_write_register(MPR121_ELE0_T, TOU_THRESH);
	mpr121_write_register(MPR121_ELE0_R, REL_THRESH);
	mpr121_write_register(MPR121_ELE1_T, TOU_THRESH);
	mpr121_write_register(MPR121_ELE1_R, REL_THRESH);
	mpr121_write_register(MPR121_ELE2_T, TOU_THRESH);
	mpr121_write_register(MPR121_ELE2_R, REL_THRESH);
	mpr121_write_register(MPR121_ELE3_T, TOU_THRESH);
	mpr121_write_register(MPR121_ELE3_R, REL_THRESH);
	mpr121_write_register(MPR121_ELE4_T, TOU_THRESH);
	mpr121_write_register(MPR121_ELE4_R, REL_THRESH);
	mpr121_write_register(MPR121_ELE5_T, TOU_THRESH);
	mpr121_write_register(MPR121_ELE5_R, REL_THRESH);
	
	mpr121_write_register(MPR121_ELE6_T, TOU_THRESH);
	mpr121_write_register(MPR121_ELE6_R, REL_THRESH);
	mpr121_write_register(MPR121_ELE7_T, TOU_THRESH);
	mpr121_write_register(MPR121_ELE7_R, REL_THRESH);
	mpr121_write_register(MPR121_ELE8_T, TOU_THRESH);
	mpr121_write_register(MPR121_ELE8_R, REL_THRESH);
	mpr121_write_register(MPR121_ELE9_T, TOU_THRESH);
	mpr121_write_register(MPR121_ELE9_R, REL_THRESH);
	mpr121_write_register(MPR121_ELE10_T, TOU_THRESH);
	mpr121_write_register(MPR121_ELE10_R, REL_THRESH);
	mpr121_write_register(MPR121_ELE11_T, TOU_THRESH);
	mpr121_write_register(MPR121_ELE11_R, REL_THRESH);
	
	// Section D
	// Set the Filter Configuration
	// Set ESI2
	mpr121_write_register(MPR121_FIL_CFG, 0x04);
	
	// Section E
	// Electrode Configuration
	// Enable 6 Electrodes and set to run mode
	// Set ELE_CFG to 0x00 to return to standby mode
	 mpr121_write_register(MPR121_ELE_CFG, 0x0C);	// Enables all 12 Electrodes
	//mpr121_write_register(MPR121_ELE_CFG, 0x06);		// Enable first 6 electrodes
	
	// Section F
	// Enable Auto Config and auto Reconfig
	mpr121_write_register(MPR121_AUTO_CFG0, 0x0B);
	mpr121_write_register(MPR121_AUTO_CFGU, 0xC9);	// USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V
	mpr121_write_register(MPR121_AUTO_CFGL, 0x82);	// LSL = 0.65*USL = 0x82 @3.3V
	mpr121_write_register(MPR121_AUTO_CFGT, 0xB5);	// Target = 0.9*USL = 0xB5 @3.3V
	
}

void mpr121_config_prox_13(void)
{
	 
	mpr121_write_register(MPR121_BASELINE_PROX, 0x01);	//Clear Status
	mpr121_write_register(MPR121_DEBOUNCE,	0x33);			//Debounce
	mpr121_write_register(MPR121_PROX_T, 8);					//Touch thresh
	mpr121_write_register(MPR121_PROX_R, 5);					//Release thresh
	mpr121_write_register(MPR121_PROX_MHD_R, 0xFF);
	mpr121_write_register(MPR121_PROX_NHD_R, 0xFF);
	mpr121_write_register(MPR121_PROX_NCL_R, 0);
	mpr121_write_register(MPR121_PROX_FDL_R, 0);
	mpr121_write_register(MPR121_PROX_MHD_F, 0x01);
	mpr121_write_register(MPR121_PROX_NHD_F, 0x01);
	mpr121_write_register(MPR121_PROX_NCL_F, 0xFF);
	mpr121_write_register(MPR121_PROX_FDL_F, 0xFF);
	mpr121_write_register(MPR121_PROX_NHD_T, 0x00);
	mpr121_write_register(MPR121_PROX_NCL_T, 0x00);
	mpr121_write_register(MPR121_PROX_FDL_T, 0x00);
	mpr121_write_register(MPR121_FIL_CDC, 0xC1);
	mpr121_write_register(MPR121_FIL_CFG, 0xFC);
	mpr121_write_register(MPR121_ELE_CFG, 0xD2);	// Enables all 12 Electrodes
/*	mpr121_write_register(MPR121_AUTO_CFG0, 0x0B);
	mpr121_write_register(MPR121_AUTO_CFGU, 0x9C);	// USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V
	mpr121_write_register(MPR121_AUTO_CFGL, 0x65);	// LSL = 0.65*USL = 0x82 @3.3V
	mpr121_write_register(MPR121_AUTO_CFGT, 0x8C);	// Target = 0.9*USL = 0xB5 @3.3V 
*/

}

void mpr121_config_prox_touch_0(void)
{
	 /*
	mpr121_write_register(MPR121_BASELINE_PROX, 0x01);	//Clear Status
	mpr121_write_register(MPR121_DEBOUNCE,	0x33);			//Debounce
	mpr121_write_register(MPR121_ELE0_T, 0x96);
	mpr121_write_register(MPR121_ELE0_R, 0x64);
	mpr121_write_register(MPR121_FIL_CDC, 0xC1);
	mpr121_write_register(MPR121_FIL_CFG, 0xF8);
	mpr121_write_register(MPR121_ELE_CFG, 0xC1);
/*	mpr121_write_register(MPR121_AUTO_CFG0, 0x0B);
	mpr121_write_register(MPR121_AUTO_CFGU, 0x9C);	// USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V
	mpr121_write_register(MPR121_AUTO_CFGL, 0x65);	// LSL = 0.65*USL = 0x82 @3.3V
	mpr121_write_register(MPR121_AUTO_CFGT, 0x8C);	// Target = 0.9*USL = 0xB5 @3.3V 
*/
mpr121_write_register(MPR121_BASELINE_PROX, 0x01);	//Clear Status
	mpr121_write_register(MPR121_DEBOUNCE,	0x11);			//Debounce
	mpr121_write_register(MPR121_ELE0_T, 0x96);
	mpr121_write_register(MPR121_ELE0_R, 0x64);
	mpr121_write_register(MPR121_FIL_CDC, 0xC1);
	mpr121_write_register(MPR121_FIL_CFG, 0xD8);
	mpr121_write_register(MPR121_ELE_CFG, 0xC1);
}