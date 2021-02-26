/* Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef VBAT_MAP_API_H
#define VBAT_MAP_API_H

#define MAX_VBAT_MONITOR_WRITES 48

#define VBAT_ADC_N0          8                 // ADC Bit-width inside Vbat monitor
#define VBAT_ADC_MIN         0                 // ADC code min
#define VBAT_ADC_MAX         2047              // ADC code max
#define VBAT_ADC_REF1        280               // 11-bit Vbat ADC value for 0.8595 bandgap reference voltage
#define VBAT_ADC_REF2        830               // 11-bit Vbat ADC value for 1.0744 bandgap reference voltage

//Range of ADC output Dcp1, Dcp2 values for bandgap voltage of 0.85, 1.07 respectively
#define VBAT_ADC_DCP1_MIN    248
#define VBAT_ADC_DCP1_MAX    400
#define VBAT_ADC_DCP2_MIN    730
#define VBAT_ADC_DCP2_MAX    918

// Range Values for parameters
#define VBAT_ADC_SLOPE_MIN   0
#define VBAT_ADC_SLOPE_MAX   1023               // Max value of Vbat ADC slope
#define VBAT_ADC_BIAS_MIN    -256
#define VBAT_ADC_BIAS_MAX    255                // Max value of Vbat ADC bias
#define VBAT_ALPHA_MIN       0x0                // Alpha select parameter
#define VBAT_ALPHA_MAX       0x07
#define VBAT_RX_BLOCK_MIN    0x02               // Rx block window size parameter
#define VBAT_RX_BLOCK_MAX    0x64
#define VBAT_RX_THRESH_MIN   0x0                // Rx signal peak threshold parameter
#define VBAT_RX_THRESH_MAX   0xC8
#define VBAT_RF_VDOWN_MIN    0x0                // RF Vbat down parameter
#define VBAT_RF_VDOWN_MAX    0x100
#define VBAT_RF_TRIP_MIN     0x240              // RF Trip point threshold parameter
#define VBAT_RF_TRIP_MAX     0x4A6
#define VBAT_TRIP_MIN        0x240              // Vbat Trip point threshold parameter
#define VBAT_TRIP_MAX        0x4A6
#define VBAT_MAX_ATTEN_MIN   0x0                // Max attenuation parameter
#define VBAT_MAX_ATTEN_MAX   0x24
#define VBAT_MAX_LIM_MIN     0x0                // Max limiter level parameter
#define VBAT_MAX_LIM_MAX     0x24
#define VBAT_SLOPE_MIN       0x01               // Vbat Slope parameter
#define VBAT_SLOPE_MAX       0x24
#define VBAT_ATTACK_MIN      0x01                // Vbat Attack time parameter
#define VBAT_ATTACK_MAX      0xFFFF
#define VBAT_RELEASE_MIN     0x0A                // Vbat Release time parameter
#define VBAT_RELEASE_MAX     0xFFFF
#define VBAT_DBSTEP_MIN      0x01                // Vbat dBStep parameter
#define VBAT_DBSTEP_MAX      0x06

#define VBAT_REG_MASK        0xFF
/* Bitmasks */
#define WCD9335_CDC_VBAT_VBAT_CFG__VBAT_MODE_SEL         (1 << 1)

/* WCD9335 registers */
#define WCD9335_CDC_VBAT_VBAT_PATH_CTL                   0x0c3d
#define WCD9335_CDC_VBAT_VBAT_CFG                        0x0c3e
#define WCD9335_CDC_VBAT_VBAT_ADC_CAL1                   0x0c3f
#define WCD9335_CDC_VBAT_VBAT_ADC_CAL2                   0x0c40
#define WCD9335_CDC_VBAT_VBAT_ADC_CAL3                   0x0c41
#define WCD9335_CDC_VBAT_VBAT_PK_EST1                    0x0c42
#define WCD9335_CDC_VBAT_VBAT_PK_EST2                    0x0c43
#define WCD9335_CDC_VBAT_VBAT_PK_EST3                    0x0c44
#define WCD9335_CDC_VBAT_VBAT_RF_PROC1                   0x0c45
#define WCD9335_CDC_VBAT_VBAT_RF_PROC2                   0x0c46
#define WCD9335_CDC_VBAT_VBAT_TAC1                       0x0c47
#define WCD9335_CDC_VBAT_VBAT_TAC2                       0x0c48
#define WCD9335_CDC_VBAT_VBAT_TAC3                       0x0c49
#define WCD9335_CDC_VBAT_VBAT_TAC4                       0x0c4a
#define WCD9335_CDC_VBAT_VBAT_GAIN_UPD1                  0x0c4b
#define WCD9335_CDC_VBAT_VBAT_GAIN_UPD2                  0x0c4c
#define WCD9335_CDC_VBAT_VBAT_GAIN_UPD3                  0x0c4d
#define WCD9335_CDC_VBAT_VBAT_GAIN_UPD4                  0x0c4e



/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


// Structure Definitions

typedef struct vbat_monitor_reg_t {
	uint32_t size;
    // Size represents total number of valid elements in the array writes[MAX_VBAT_MONITOR_WRITES];
	uint32_t writes[MAX_VBAT_MONITOR_WRITES];
    // 16 bits of write[x] applicable for api convert_vbat_monitor_acdb_to_reg(...) & convert_vbat_monitor_reg_to_acdb(...)
    // is partitioned as [reg (msb 16 bits), mask(8 bits) , val (8 bits)]
} vbat_monitor_reg_t;

typedef struct vbat_adc_data{
	uint16_t dcp1;
	uint16_t dcp2;
}vbat_adc_data_t;

typedef struct vbat_static_cfg_t
{
    int32_t rx_pcm_rate;                   // Rx PCM Sampling rate
    int32_t adc_rate;                      // ADC clock rate

} vbat_static_cfg_t;

typedef struct vbat_adc_cal_cfg_t
{
    uint16_t Version;
    uint16_t Auto_Cal_Enable;            // Automatic calibration enable
    uint16_t D_Slope;                    // Slope parameter
    int16_t D_Bias;                      // Bias parameter

} vbat_adc_cal_cfg_t;

typedef struct vbat_gain_proc_cfg_t
{
    uint16_t Version;
    uint16_t Vbat_Mode;
    uint16_t Vbat_Alpha_Select;
    uint16_t Vbat_Rx_Peak_Block_Select;
    int16_t Vbat_Rx_Signal_Threshold;
    uint16_t Vbat_RF_Vbat_Down;
    uint16_t Vbat_RF_Vbat_Trip_Point;
    uint16_t Vbat_Trip_Point;
    uint16_t Vbat_Max_Attenuation;
    int16_t Vbat_Max_Limiter_Level;
    uint16_t Vbat_Slope_Select;
    uint16_t Vbat_Attack_Time;
    uint16_t Vbat_Release_Time;
    uint16_t Vbat_dBStep;

} vbat_gain_proc_cfg_t;


typedef struct vbat_monitor_acdb_cfg_t
{
   vbat_adc_cal_cfg_t adc_cal_cfg;                  // ADC Calibration CFG
   vbat_gain_proc_cfg_t gain_proc_cfg;              // Vbat Gain processing CFG

}vbat_monitor_acdb_cfg_t;


/*----------------------------------------------------------------------------
 * Type Declarations for overall configuration and state data structures
 * -------------------------------------------------------------------------*/

//***************** Function Definitions
//
// Set/Write to Vbat monitor HW registers
// Out: vbat_monitor_reg_config_ptr --> pointer to the Vbat HW register structure with updated parameter values.
// In: vbat_monitor_acdb_ptr --> pointer to the Vbat ACDB structure with tuning values.
return_code_t convert_vbat_monitor_acdb_to_reg(struct vbat_monitor_reg_t *vbat_monitor_reg_config_ptr, int8_t *vbat_monitor_acdb_ptr, vbat_adc_data_t *VbatACDBdata);

// Get/Read from Vbat monitor HW registers
// In: vbat_monitor_reg_config_ptr --> pointer to the Vbat HW register structure with parameter values.
// Out: vbat_monitor_acdb_ptr --> pointer to the Vbat ACDB structure with updated tuning values.
return_code_t convert_vbat_monitor_reg_to_acdb(int8_t *vbat_monitor_acdb_ptr, struct vbat_monitor_reg_t *vbat_monitor_reg_config_ptr );


// Function to perform Range check on uint16_t parameters
// Out: Range checked parameter value
// in_param: uint16_t input parameter
// min_val:  Minimum bound value for the parameter
// max_val: Maximum bound value for the parameter

uint16_t vbat_range_check(uint16_t in_param, uint16_t min_val, uint16_t max_val);

// Function to convert differential voltage value to differential ADC value
// Out: Differential ADC value
// vbat_diff_in: uint16_t input differential Vbat voltage
// nbits: Bitwidth 8 or 11

uint16_t vbat_adc_diff(uint16_t vbat_diff_in, uint16_t nbits);

// Function to convert Vbat voltage value to Vbat ADC value
// Out: Vbat ADC value
// vbat_in: uint16_t input Vbat voltage
// nbits: Bitwidth 8 or 11

uint16_t vbat_adc(uint16_t vbat_in, uint16_t nbits);

// Function to convert Vbat ADC diff value to Vbat voltage value
// Out: Vbat diff voltage value
// vbat_in: uint16_t input Vbat ADC diff value
// nbits: Bitwidth 8 or 11

uint16_t vbat_adc_diff_voltage(uint16_t vadc_diff_in, uint16_t nbits);

// Function to convert Vbat ADC value to Vbat voltage value
// Out: Vbat voltage value
// vbat_in: uint16_t input Vbat ADC value
// nbits: Bitwidth 8 or 11

uint16_t vbat_adc_voltage(uint16_t vadc_in, uint16_t nbits);

// Function to compute the ADC calibration parameters
// Dcp1: uint16_t input containing 11-bit Vbat ADC measurement for the first bandgap voltage (0.85 V)
// Dcp2: uint16_t input containing 11-bit Vbat ADC measurement for the second bandgap voltage (1.05 V)
// &D_Slope:  Pointer to the Vbat ADC calibration slope parameter with updated value
// &D_Bias:   Pointer to the Vbat ADC calibration bias parameter with updated value

void vbat_adc_cal_compute(uint16_t Dcp1, uint16_t Dcp2, uint16_t* D_Slope, int16_t* D_Bias);

// Function to convert VBAT calibration parameters to HW register address and value pairs
// Out: vbat_monitor_reg_config_ptr --> pointer to the Vbat HW register structure with updated parameter values.
// In: vbat_monitor_acdb_ptr --> pointer to the Vbat ACDB structure with tuning values.
// In: VbatADCdata --> pointer to the Vbat ACDB data with tuning values.
return_code_t vbat_conversion(vbat_monitor_reg_t *vbat_monitor_reg_config_ptr, int *vbat_monitor_acdb_ptr, vbat_adc_data_t *VbatADCdata);

#endif /* #ifndef VBAT_MAP_API_H */
