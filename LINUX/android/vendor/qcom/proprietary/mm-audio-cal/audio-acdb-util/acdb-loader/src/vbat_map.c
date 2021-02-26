/* Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "vbat_map"
/*#define LOG_NDEBUG 0*/
#define LOG_NDDEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "anc_map_api.h"
#include "vbat_map_api.h"

#ifdef _ANDROID_
#include <cutils/properties.h>
#include <utils/Log.h>
#include "common_log.h"
#else
#define LOGI(...)      fprintf(stdout,__VA_ARGS__)
#define LOGE(...)      fprintf(stderr,__VA_ARGS__)
#define LOGV(...)      fprintf(stderr,__VA_ARGS__)
#define LOGD(...)      fprintf(stderr,__VA_ARGS__)
#endif

/* Maintain CSIM version number */
#define VBAT_MONITOR_CSIM_VERSION "01.00.00"

#define IS_IN_RANGE(X,Y,Z) ((X > Y && X < Z)? 1 : 0)
/*----------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/
static return_code_t is_valid_auto_cal(vbat_adc_data_t *VbatADCdata)
{
	if (!VbatADCdata)
		return INVALID_PTR;

	if (IS_IN_RANGE(VbatADCdata->dcp1, VBAT_ADC_DCP1_MIN, VBAT_ADC_DCP1_MAX) &&
		IS_IN_RANGE(VbatADCdata->dcp2, VBAT_ADC_DCP2_MIN, VBAT_ADC_DCP2_MAX))
		return SUCCESS;
	else
		return FAIL;
}

// Set to Vbat monitor HW registers
// Out: vbat_monitor_reg_config_ptr --> pointer to the Vbat HW register structure with updated parameter values.
// In: vbat_monitor_acdb_ptr --> pointer to the Vbat ACDB structure with tuning values.

return_code_t convert_vbat_monitor_acdb_to_reg(struct vbat_monitor_reg_t *vbat_monitor_reg_config_ptr, int8_t *vbat_monitor_acdb_ptr, vbat_adc_data_t *VbatADCdata)
{

	uint32_t rx_pcm_rate, adc_rate, tmp32;        // Rx PCM and ADC clock rate
	uint16_t  rx_pcm_KHz, adc_rate_KHz, dSlope, rx_block, alphas, dBStep, slope;
	uint16_t  reg_indx=0;                   // Vbat HW register counter
	uint16_t  tmp16, atmp16, btmp16;
	uint8_t  tmp1, tmp2, tmp3;
	int16_t rx_thresh, dBias;
	float rx_thresh_f, tmpf;

	vbat_monitor_acdb_cfg_t *pVbatCfg = (vbat_monitor_acdb_cfg_t *)vbat_monitor_acdb_ptr;

	// Invalid pointer check and return error
	if ((NULL == vbat_monitor_acdb_ptr) || (NULL == vbat_monitor_reg_config_ptr))
		return(INVALID_PTR);

	/* Hardcoded  Codec CFG rates, rx_pcm_rates */
	adc_rate =  2400000;
	adc_rate_KHz = (uint16_t)(((float)adc_rate)*0.001);

	rx_pcm_rate = 48000;
	rx_pcm_KHz = (uint16_t)(((float)rx_pcm_rate)*0.001);

	tmp1 = (uint8_t)pVbatCfg->gain_proc_cfg.Vbat_Mode;

	// Write the Vbat CFG mode word
	vbat_monitor_reg_config_ptr->writes[reg_indx++] =  REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_CFG, WCD9335_CDC_VBAT_VBAT_CFG__VBAT_MODE_SEL, tmp1 << 1);

	// Check the auto-calibration flag for ADC calibration
	// is_valid_auto_cal() returns '0' upon success
	if (pVbatCfg->adc_cal_cfg.Auto_Cal_Enable == 1 && !is_valid_auto_cal(VbatADCdata)) {
	// HLOS SW measures the ADC outputs with bandgap voltages and computes the ADC calibration automatically
	// during HW codec initialization
	// Call the function: vbat_adc_cal_compute(uint16_t  Dcp1, uint16_t  Dcp2, uint16* D_Slope, int16* D_Bias)
	// to compute the ADC calibration factors from the bandgap voltage measurements.
		vbat_adc_cal_compute(VbatADCdata->dcp1, VbatADCdata->dcp2, &dSlope, &dBias);
	} else {
		// Allow the user to manually over-ride the ADC calibration through QACT.
		dSlope = pVbatCfg->adc_cal_cfg.D_Slope;
		dBias = pVbatCfg->adc_cal_cfg.D_Bias;
	}

	// Write the Vbat D_Slope 8 bits LSB
	tmp16 = (dSlope & 0x00FF);
	tmp1 = (uint8_t)(tmp16);
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_ADC_CAL1, VBAT_REG_MASK, tmp1);

	// Write the Vbat D_Bias 8 bits LSB
	atmp16 = (dBias & 0x00FF);
	tmp1 = (uint8_t)(atmp16);
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_ADC_CAL2, VBAT_REG_MASK, tmp1);

	// Range Check
	alphas = pVbatCfg->gain_proc_cfg.Vbat_Alpha_Select;
	alphas = vbat_range_check(alphas, VBAT_ALPHA_MIN, VBAT_ALPHA_MAX);

	// Write the Vbat Alpha Select + D_Slope/D_Bias residual
	tmp1 = (uint8_t)(alphas << 3);

	tmp16 = (dSlope & 0x300);
	tmp16 = (tmp16 >> 7);
	tmp2 = (uint8_t)tmp16;

	tmp16 = (dBias & 0x100);
	tmp16 = (tmp16 >> 8);
	tmp3 = (uint8_t)tmp16;

	tmp1 = (tmp1+tmp2+tmp3);
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_ADC_CAL3, VBAT_REG_MASK, tmp1);

	// Rx block size in number of samples
	rx_block = vbat_range_check(pVbatCfg->gain_proc_cfg.Vbat_Rx_Peak_Block_Select, VBAT_RX_BLOCK_MIN, VBAT_RX_BLOCK_MAX);
	tmp16 = (rx_block*rx_pcm_KHz) >> 1;    // Convert from Q1 to Q0

	// Separate Rx block into 8 LSBs and 6 MSBs
	tmp1 = (uint8_t)tmp16;	// 8 LSBs
	tmp2 = (uint8_t)( tmp16 >> 8);
	tmp2 = (tmp2 & 0x3F);	// 6 MSBs

	// Write the Rx block size into the Vbat registers
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_PK_EST1, VBAT_REG_MASK, tmp1);
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_PK_EST2, VBAT_REG_MASK, tmp2);

	// Compute the Rx peak signal level
	// Range Check
	rx_thresh = vbat_range_check(pVbatCfg->gain_proc_cfg.Vbat_Rx_Signal_Threshold, VBAT_RX_THRESH_MIN, VBAT_RX_THRESH_MAX);

	// Convert from Q1 to Q0 and do 10^(-rx_thresh/20)
	rx_thresh_f = -((float)rx_thresh)/40;
	rx_thresh_f = (float)pow((double)10, (double)rx_thresh_f);

	tmp16 = (uint16_t)(rx_thresh_f*256 + 0.5);	// Round down to integer in 8 bit format
	tmp16 = vbat_range_check(tmp16, 0, 255);

	tmp1 = (uint8_t)tmp16;
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_PK_EST3, VBAT_REG_MASK, tmp1);

	// Compute RF Vbat down parameter
	// Range Check implementation
	atmp16 = vbat_range_check(pVbatCfg->gain_proc_cfg.Vbat_RF_Vbat_Down, VBAT_RF_VDOWN_MIN, VBAT_RF_VDOWN_MAX);

	// Convert ACDB value to register value
	btmp16 = vbat_adc_diff(atmp16, VBAT_ADC_N0);
	tmp1 = (uint8_t)btmp16;
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_RF_PROC1, VBAT_REG_MASK, tmp1);

	// Compute RF Vbat Trip point parameter
	// Range Check implementation
	atmp16 = vbat_range_check(pVbatCfg->gain_proc_cfg.Vbat_RF_Vbat_Trip_Point, VBAT_RF_TRIP_MIN, VBAT_RF_TRIP_MAX);

	// Convert ACDB value to register value
	btmp16 = vbat_adc(atmp16, VBAT_ADC_N0);
	tmp1 = (uint8_t)btmp16;
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_RF_PROC2, VBAT_REG_MASK, tmp1);

	// Compute Vbat Trip point parameter
	// Range Check implementation
	atmp16 = vbat_range_check(pVbatCfg->gain_proc_cfg.Vbat_Trip_Point, VBAT_TRIP_MIN, VBAT_TRIP_MAX);

	// Convert ACDB value to register value
	btmp16 = vbat_adc(atmp16, VBAT_ADC_N0);
	tmp1 = (uint8_t)btmp16;
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_TAC1, VBAT_REG_MASK, tmp1);

	// Set Max attenuation parameter
	// Range Check implementation
	atmp16 = vbat_range_check(pVbatCfg->gain_proc_cfg.Vbat_Max_Attenuation, VBAT_MAX_ATTEN_MIN, VBAT_MAX_ATTEN_MAX);
	tmp1 = (uint8_t)atmp16;
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_TAC2, VBAT_REG_MASK, tmp1);

	// Set Max limiter parameter
	// Range Check implementation
	btmp16 = vbat_range_check(pVbatCfg->gain_proc_cfg.Vbat_Max_Limiter_Level, VBAT_MAX_LIM_MIN, VBAT_MAX_LIM_MAX);
	tmp1 = (uint8_t)btmp16;
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_TAC3, VBAT_REG_MASK, tmp1);

	// Set Slope parameter
	// Range Check implementation
	slope = vbat_range_check(pVbatCfg->gain_proc_cfg.Vbat_Slope_Select, VBAT_SLOPE_MIN, VBAT_SLOPE_MAX);

	// Convert Slope value to Slope select
	atmp16 = (slope >> 1);
	if (atmp16 > 15)
		atmp16 = 0x0D;               // 18 dB/0.1V slope
        else if (atmp16 > 12)
		atmp16 = 0x0C;               // 15.33 dB/0.1V slope
        else if (atmp16 > 10)
		atmp16 = 0x0B;               // 12 dB/0.1V slope

	tmp1 = (uint8_t)atmp16;
	// Write to the register
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_TAC4, VBAT_REG_MASK, tmp1);

	// Get the dBStep parameter
	dBStep = vbat_range_check(pVbatCfg->gain_proc_cfg.Vbat_dBStep, VBAT_DBSTEP_MIN, VBAT_DBSTEP_MAX);
	// Set Attack time parameter
	// Range Check implementation
	atmp16 = vbat_range_check(pVbatCfg->gain_proc_cfg.Vbat_Attack_Time, VBAT_ATTACK_MIN, VBAT_ATTACK_MAX);

	tmp32 = ((uint32_t)atmp16 * adc_rate_KHz * dBStep);
	btmp16 = (uint16_t)( (uint32_t)tmp32 >> 15);               // Convert from Q14 ACDB value to Q0 register value
	//Do not let attack time go to 0
	if (btmp16 < 1)
		btmp16 = 1;

	// Separate attack time into 8 LSBs and 7 MSBs
	tmp1 = (uint8_t)btmp16;// 8 LSBs
	tmp2 = (uint8_t)( btmp16 >> 8);
	tmp2 = (tmp2 & 0x7F);	// 7 MSBs

	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_GAIN_UPD1, VBAT_REG_MASK, tmp1);
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_GAIN_UPD2, VBAT_REG_MASK, tmp2);

	// Set Release time parameter
	// Range Check implementation
	atmp16 = vbat_range_check(pVbatCfg->gain_proc_cfg.Vbat_Release_Time, VBAT_RELEASE_MIN, VBAT_RELEASE_MAX);

	// Multiply release time by 1e-3*200*0.5 (=0.1) to get the release counter max
	tmpf = (float)( (float)atmp16 * 0.1);
	btmp16 = (uint16_t)(tmpf + 0.5);           // Round down the release counter value
	btmp16 = (btmp16 >> 5);                    // Convert from Q5 ACDB value to Q0 register value
	//Do not let release time go to 0
	if (btmp16 < 1)
		btmp16 = 1;

	// Separate Release time into 8 bit value
	tmp1 = (uint8_t)btmp16;              // 8 LSBs
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_GAIN_UPD3, VBAT_REG_MASK, tmp1);

	tmp1 = (uint8_t)dBStep;
	// Write the dBStep value to the register
	vbat_monitor_reg_config_ptr->writes[reg_indx++] = REG_CODEC_PACK_ENTRY(WCD9335_CDC_VBAT_VBAT_GAIN_UPD4, VBAT_REG_MASK, tmp1);

	vbat_monitor_reg_config_ptr->size = reg_indx;

	LOGV("Vbat Registers Size: %d \n", vbat_monitor_reg_config_ptr->size);
	return(SUCCESS);
}


// Get/Read from Vbat monitor HW registers
// In: vbat_monitor_reg_config_ptr --> pointer to the Vbat HW register structure with parameter values.
// Out: vbat_monitor_acdb_ptr --> pointer to the Vbat ACDB structure with updated tuning values.
return_code_t convert_vbat_monitor_reg_to_acdb(int8_t *vbat_monitor_acdb_ptr, struct vbat_monitor_reg_t *vbat_monitor_reg_config_ptr )
{
	uint32_t rx_pcm_rate, adc_rate, tmp32;        // Rx PCM and ADC clock rate
	uint16_t  rx_pcm_KHz, adc_rate_KHz, rx_block, dBStep;
	uint16_t  reg_indx=0;                   // Vbat HW register counter
	uint16_t  tmp16, atmp16, btmp16;
	uint8_t  tmp1, tmp2, tmp3;
	int16_t rx_thresh, stmp16;
	float rx_block_f, rx_thresh_f, ftmp;

	vbat_monitor_acdb_cfg_t *pVbatCfg = (vbat_monitor_acdb_cfg_t *)vbat_monitor_acdb_ptr;

	// Invalid pointer check and return error
	if ((NULL == vbat_monitor_acdb_ptr) || (NULL == vbat_monitor_reg_config_ptr))
		return(INVALID_PTR);

	// Codec CFG rates
	adc_rate = 2400000;//pVbatCfg->codec_cfg.adc_rate;
	adc_rate_KHz = (uint16_t)(((float)adc_rate)*0.001);      // ADC rate in KHz

	rx_pcm_rate = 48000; //pVbatCfg->codec_cfg.rx_pcm_rate;
	rx_pcm_KHz = (uint16_t)(((float)rx_pcm_rate)*0.001);     // Rx PCM rate in KHz

	// Read the Vbat CFG mode word
	pVbatCfg->gain_proc_cfg.Vbat_Mode = (uint16_t)vbat_monitor_reg_config_ptr->writes[reg_indx++];

	// Vbat monitor ADC calibration
	tmp1 = vbat_monitor_reg_config_ptr->writes[reg_indx++];     // ADC D_Slope LSB 8 bits
	tmp2 = vbat_monitor_reg_config_ptr->writes[reg_indx++];     // ADC D_Bias LSB 8 bits
	tmp3 = vbat_monitor_reg_config_ptr->writes[reg_indx++];     // ADC Alpha Select + MSB ADC Slope + MSB ADC BIas

	// ADC D_Slope
	tmp16 = (tmp3 >>1);
	tmp16 = (tmp16 & 0x0003);
	tmp16 = (tmp16 << 8);
	pVbatCfg->adc_cal_cfg.D_Slope = (tmp16 + (uint16_t)tmp1);     // ADC D_Slope in 10 bit format

	// ADC D_Bias
	tmp16 = (tmp3 & 0x01);
	if (tmp16 == 1)
		stmp16 = tmp2 - 256;             // Negative value for D_Bias
	else
		stmp16 = tmp2;                   // Positive value for D_Bias

	pVbatCfg->adc_cal_cfg.D_Bias = stmp16;                       // ADC D_Bias in signed Q1.8 format

	// Vbat Alpha Select
	pVbatCfg->gain_proc_cfg.Vbat_Alpha_Select = (uint16_t)(tmp3 >> 3);

	// Rx Peak block select
	atmp16 = vbat_monitor_reg_config_ptr->writes[reg_indx++];
	btmp16 = vbat_monitor_reg_config_ptr->writes[reg_indx++];
	btmp16 = (btmp16 & 0x3F);

	rx_block = ((btmp16 << 8) + atmp16);

	// Convert from Q0 to Q1 and multiply by RX_PCM in KHz to get the Rx block size
	rx_block = (rx_block << 1);
	rx_block_f = ((float)rx_block/rx_pcm_KHz);
	rx_block = (uint16_t)(rx_block_f + 0.5);

	pVbatCfg->gain_proc_cfg.Vbat_Rx_Peak_Block_Select = rx_block;

	// Rx Signal Threshold in Q8 format (10^(-rx_thresh/20))
	rx_thresh = vbat_monitor_reg_config_ptr->writes[reg_indx++];
	rx_thresh_f = (float)( ((float)rx_thresh)/256.0);

	rx_thresh_f = (float)log10((double)rx_thresh_f);
	rx_thresh_f = (float)(-40.0 * rx_thresh_f);                 // Convert to Q1 format
	rx_thresh = (uint16_t)(rx_thresh_f+0.5);
	pVbatCfg->gain_proc_cfg.Vbat_Rx_Signal_Threshold = rx_thresh;

	// RF Vbat down parameter
	tmp16 = vbat_monitor_reg_config_ptr->writes[reg_indx++];
	atmp16 = vbat_adc_diff_voltage(tmp16, VBAT_ADC_N0);
	pVbatCfg->gain_proc_cfg.Vbat_RF_Vbat_Down = atmp16;

	// RF Vbat trip point
	tmp16 = vbat_monitor_reg_config_ptr->writes[reg_indx++];
	atmp16 = vbat_adc_voltage(tmp16, VBAT_ADC_N0);
	pVbatCfg->gain_proc_cfg.Vbat_RF_Vbat_Trip_Point = atmp16;

	// Vbat trip point
	tmp16 = vbat_monitor_reg_config_ptr->writes[reg_indx++];
	atmp16 = vbat_adc_voltage(tmp16, VBAT_ADC_N0);
	pVbatCfg->gain_proc_cfg.Vbat_Trip_Point = atmp16;

	// Vbat Max attenuation
	tmp16 = vbat_monitor_reg_config_ptr->writes[reg_indx++];
	pVbatCfg->gain_proc_cfg.Vbat_Max_Attenuation = tmp16;

	// Vbat Max Limiter Level
	tmp16 = vbat_monitor_reg_config_ptr->writes[reg_indx++];
	pVbatCfg->gain_proc_cfg.Vbat_Max_Limiter_Level = tmp16;

	// Vbat Slope parameter
	tmp16 = vbat_monitor_reg_config_ptr->writes[reg_indx++];

	if (tmp16 >= 0x0D)
		atmp16 = 36;              // 18 dB/0.1V
	else if (tmp16 == 0x0C)
		atmp16 = 31;              // 15.33 dB/0.1V
	else if (tmp16 == 0x0B)
		atmp16 = 24;              // 12 dB/ 0.1V
	else if (tmp16 == 0x00)
		atmp16 = 1;
	else
		atmp16 = (tmp16 << 1);     // Rest of the slope values

	pVbatCfg->gain_proc_cfg.Vbat_Slope_Select = atmp16;

	// Vbat attack and release parameter registers
	tmp1 = vbat_monitor_reg_config_ptr->writes[reg_indx++];
	tmp2 = vbat_monitor_reg_config_ptr->writes[reg_indx++];
	tmp3 = vbat_monitor_reg_config_ptr->writes[reg_indx++];

	// Vbat dBStep parameter
	dBStep = vbat_monitor_reg_config_ptr->writes[reg_indx++];
	pVbatCfg->gain_proc_cfg.Vbat_dBStep = dBStep;

	// Vbat attack time computation
	tmp16 = (uint16_t)(tmp2 & 0x7F);
	tmp16 = ((tmp16 << 8) + tmp1);
	tmp32 = (uint32_t)tmp16;
	tmp32 = (tmp32 << 15);                            // Attack time in Q14 format

	ftmp = ((float)tmp32)/((float)adc_rate_KHz);
	ftmp = ftmp/((float)dBStep);

	atmp16 = (uint16_t)(ftmp+0.5);
	pVbatCfg->gain_proc_cfg.Vbat_Attack_Time = atmp16;

	// Vbat release time computation
	tmp32 = tmp3;
	tmp32 = (tmp32 << 5);                              // Release time in Q5 format

	atmp16 = (uint16_t)(tmp32*10);                     // Release time in ms in Q5 format

	pVbatCfg->gain_proc_cfg.Vbat_Release_Time = atmp16;
	return(SUCCESS);
}

// Function to perform Range check on uint16_t  parameters
// Out: Range checked parameter value
// in_param: uint16_t  input parameter
// min_val:  Minimum bound value for the parameter
// max_val: Maximum bound value for the parameter

uint16_t  vbat_range_check(uint16_t  in_param, uint16_t  min_val, uint16_t  max_val)
{

	uint16_t  out_param = in_param;

	// Perform min and max range check
	// Min bound check
	if (in_param < min_val)
		out_param = min_val;

	// Max bound check
	if (in_param > max_val)
		out_param = max_val;

	return(out_param);
}

// Function to convert differential voltage value to differential ADC value
// Out: Differential ADC value
// vbat_diff_in: uint16_t  input differential Vbat voltage
// nbits: Bitwidth 8 or 11

uint16_t  vbat_adc_diff(uint16_t  vbat_diff_in, uint16_t  nbits)
{
	uint16_t  vadc_diff_out;
	float vbat_tmp;

	// Convert Vbat diff voltage in Q8 format to ADC sample value at 11 bits
	vbat_tmp = (float)(( (float)vbat_diff_in*2048)/614.4);
	vadc_diff_out = (uint16_t)(vbat_tmp + 0.5);	// Rounding from float to int16

	// Range check: 0 - 2047 limit
	vadc_diff_out = vbat_range_check(vadc_diff_out, VBAT_ADC_MIN, VBAT_ADC_MAX);

	// Convert from 11 bit Vbat ADC value to 8 bits
	if (nbits == VBAT_ADC_N0)
		vadc_diff_out = (vadc_diff_out >> 3);

	return(vadc_diff_out);
}

// Function to convert Vbat voltage value to Vbat ADC value
// Out: Vbat ADC value
// vbat_in: uint16_t  input Vbat voltage
// nbits: Bitwidth 8 or 11

uint16_t  vbat_adc(uint16_t  vbat_in, uint16_t  nbits)
{
	uint16_t  vadc_out, vtmp, srnd;
	float vbat_tmp;

	vtmp = vbat_in - VBAT_TRIP_MIN;
	if (vtmp < 0.0)
		vtmp = 0.0;

	// Convert Vbatvoltage in Q8 format to ADC sample value at 11 bits
	vbat_tmp = (float)(( (float)vtmp*2048)/614.4);
	vadc_out = (uint16_t)(vbat_tmp + 0.5);             // Rounding from float to int16

	if (nbits == VBAT_ADC_N0) {
		srnd = (((uint16_t)1 << (11-nbits))-1);          // Add rounding before truncating from 11 to 8 bits
		vadc_out = vadc_out + srnd;
	}

	// Range check: 0 - 2047 limit
	vadc_out = vbat_range_check(vadc_out, VBAT_ADC_MIN, VBAT_ADC_MAX);

	// Convert from 11 bit Vbat ADC value to 8 bits
	if (nbits == VBAT_ADC_N0)
		vadc_out = (vadc_out >> 3);

	return(vadc_out);
}

// Function to convert Vbat ADC diff value to Vbat voltage value
// Out: Vbat diff voltage value
// vbat_in: uint16_t  input Vbat ADC diff value
// nbits: Bitwidth 8 or 11

uint16_t  vbat_adc_diff_voltage(uint16_t  vadc_diff_in, uint16_t  nbits)
{
	uint16_t  vbat_diff_out;
	float vbat_tmp;

	if (nbits == VBAT_ADC_N0)
		vadc_diff_in = (vadc_diff_in << 3);

	vadc_diff_in = vbat_range_check(vadc_diff_in, VBAT_ADC_MIN, VBAT_ADC_MAX);

	// Convert Vbatvoltage in Q8 format to ADC sample value at 11 bits
	vbat_tmp = (float)( (float)vadc_diff_in*614.4)/2048;
	vbat_diff_out = (uint16_t)(vbat_tmp + 0.5);             // Rounding from float to int16

	return(vbat_diff_out);
}

// Function to convert Vbat ADC value to Vbat voltage value
// Out: Vbat voltage value
// vbat_in: uint16_t  input Vbat ADC value
// nbits: Bitwidth 8 or 11

uint16_t  vbat_adc_voltage(uint16_t  vadc_in, uint16_t  nbits)
{
	uint16_t  vbat_out;
	float vbat_tmp;

	if (nbits == VBAT_ADC_N0)
		vadc_in = (vadc_in << 3);

	// Range check: 0 - 2047 limit
	vadc_in = vbat_range_check(vadc_in, VBAT_ADC_MIN, VBAT_ADC_MAX);

	// Convert Vbatvoltage in Q8 format to ADC sample value at 11 bits
	vbat_tmp = (float)( (float)vadc_in*614.4)/2048;
	vbat_out = (uint16_t)(vbat_tmp + 0.5);             // Rounding from float to int16

	vbat_out = vbat_out + VBAT_TRIP_MIN;
	vbat_out = vbat_range_check(vbat_out, VBAT_TRIP_MIN, VBAT_TRIP_MAX);

	return(vbat_out);
}

// Function to compute the ADC calibration parameters
// Dcp1: uint16_t  input containing 11-bit Vbat ADC measurement for the first bandgap voltage (0.85 V)
// Dcp2: uint16_t  input containing 11-bit Vbat ADC measurement for the second bandgap voltage (1.05 V)
// &D_Slope:  Pointer to the Vbat ADC calibration slope parameter with updated value
// &D_Bias:   Pointer to the Vbat ADC calibration bias parameter with updated value

void vbat_adc_cal_compute(uint16_t  Dcp1, uint16_t  Dcp2, uint16_t* D_Slope, int16_t* D_Bias)
{
	int16_t Icp_diff, Dcp_diff, bias;
	uint16_t  slope;
	float slope_ratio, bias_val;

	// Impose Range check on the Bandgap voltage ADC measurements
	Dcp1 = vbat_range_check(Dcp1, VBAT_ADC_MIN, VBAT_ADC_MAX);
	Dcp2 = vbat_range_check(Dcp2, VBAT_ADC_MIN, VBAT_ADC_MAX);

	Icp_diff = (int16_t)VBAT_ADC_REF2 - VBAT_ADC_REF1;   // Icp2 - Icp1
	Dcp_diff = (int16_t)Dcp2 - Dcp1;

	// Compute the Vbat ADC Slope
	slope_ratio = ((float)Icp_diff)/((float)Dcp_diff);
	if (slope_ratio < 0.0)
		slope_ratio = 0.0;

	slope = (uint16_t)((slope_ratio*256) + 0.5);
	slope = vbat_range_check(slope, VBAT_ADC_SLOPE_MIN, VBAT_ADC_SLOPE_MAX);
	*D_Slope = slope;

	// Compute the Vbat ADC bias
	bias_val = (slope_ratio*Dcp1);
	bias = (int16_t)(bias_val+0.5);

	bias = VBAT_ADC_REF1 - bias;

	// Truncate the bias to 8 bits
	if (bias < 0)
		bias = -(-bias >> 3);
	else
		bias = (bias >> 3);

	if (bias < VBAT_ADC_BIAS_MIN)
		bias = VBAT_ADC_BIAS_MIN;
	if (bias > VBAT_ADC_BIAS_MAX)
		bias = VBAT_ADC_BIAS_MAX;

	*D_Bias = bias;
}

return_code_t vbat_conversion(vbat_monitor_reg_t *vbat_monitor_reg_config_ptr, int *vbat_monitor_acdb_ptr, vbat_adc_data_t *VbatADCdata)
{
	return_code_t result;

	result = convert_vbat_monitor_acdb_to_reg(vbat_monitor_reg_config_ptr, (int8_t *)vbat_monitor_acdb_ptr, VbatADCdata);

	return result;
}
