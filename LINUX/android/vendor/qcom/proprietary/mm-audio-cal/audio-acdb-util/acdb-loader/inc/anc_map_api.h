/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ANC_MAP_API_H
#define ANC_MAP_API_H

#include "acdb-anc-taiko.h"

#define MAX_ANC_WRITES 500

#define REG_CODEC_PACK_ENTRY(reg, mask, val) ((val & 0xff) | \
        (((mask & 0xff) << 8) | ((reg & 0xffff) << 16)))

#define REG_CODEC_UNPACK_ENTRY(packed, reg, mask, val) \
        do { \
                 ((reg) = (((packed) >> 16) & (0xffff))); \
                 ((mask) = (((packed) >> 8) & (0xff))); \
                 ((val) = ((packed) & (0xff))); \
        } while (0);

#define CODEC_PACKED_REG_SIZE sizeof(u32)

typedef struct anc_reg_t {
        uint32_t size;
    // Size represents total number of valid elements in the array writes[MAX_ANC_WRITES];
        uint32_t writes[MAX_ANC_WRITES];
    // 32 bits of write[x] applicable for api convert_anc_acdb_to_reg(...) & convert_anc_reg_to_acdb(...) is partitioned as [reg (msb 16 bits), mask(8 bits) , val (8 bits)]
    // 32 bits of write[x] applicable for api get_anc_reg_read_sequence(...) is partitioned as [reg (msb 16 bits), direction(8 bits), val (8 bits)]
} anc_reg_t;

typedef enum codec_version_t
{
      TAIKO = 0, // Applies to Taiko and tomtom
      TAPAN,
      TASHA,
      TAVIL, //applies to variants of WCD9340 without ANC HW register address change
      END
} codec_version_t;

typedef enum access_interface_t
{
      SB=0, // Slimbus interface
      I2C, // I2C interface
      END_INTERFACE
} access_interface_t;

typedef enum return_code_t
{
      SUCCESS = 0,
      NOT_ENOUGH_MEMORY = 1,
      INVALID_PTR = 2,
      INVALID_ID = 3,
      NOT_SUPPORTED = 4,
      INVALID_SEQ = 5,
      FAIL = -1
} return_code_t;

typedef enum direction_t
{
      READ, // read direction
      WRITE, // write direction
      DIR_END
} direction_t;

enum anc_versions
{
      ANC_V0, ANC_V1, ANC_V2
};

enum codec_anc_input_device {
	ADIE_MAP_ZERO = 0,
	ADIE_MAP_ADC0,
	ADIE_MAP_ADC1,
	ADIE_MAP_ADC2,
	ADIE_MAP_ADC3,
	ADIE_MAP_ADC4,
	ADIE_MAP_ADC5,
	ADIE_MAP_ADC6,   // not used in WCD9335, added for future expansion
	ADIE_MAP_DMIC0,
	ADIE_MAP_DMIC1,
	ADIE_MAP_DMIC2,
	ADIE_MAP_DMIC3,
	ADIE_MAP_DMIC4,
	ADIE_MAP_DMIC5,
	ADIE_MAP_DMIC6,  // not used in WCD9335, added for future expansion
	ADIE_MAP_INPUT_DEVICE_MAX = 0x7FFFFFFF
};

enum codec_anc_input_type {
   ADC_ANC_MIC = 0, // Analog mic input
   DMIC_ANC_MIC, // Digital Mic input
   SMTLNQ_ANC_MIC // SmartLnQ enabled Mic
};


#define ANC_HW3 0x3000
/*
 * ANC_HW3 id is associated with anc_taiko_acdb_cfg_t .
 * And Supports codec versions up to TAIKO,TAPAN.
 */

typedef struct anc_taiko_acdb_cfg_t {
	uint8_t default_enable;          //check if the anc channel is enabled or not
	uint8_t anc_feedback_enable;     //CDC_CLK_ANC_CLK_EN_CTL
	/* CDC_ANC_CTL  -- used with anc_lr_mix_output_channel, if true and 1,
	 * set it for CDC_ANC1_CTL, if true and 2, set it for CDC_ANC2_CTL, if false,
	 * set to false
	 */
	uint8_t anc_lr_mix_enable;
	uint8_t smlpf_enable;                     //CDC_ANC_CTL
	uint8_t ff_in_enable;                     //CDC_ANC_CTL
	uint8_t hybrid_enable;                    //CDC_ANC_CTL
	uint8_t ff_out_enable;                    //CDC_ANC_CTL
	uint8_t dcflt_enable;                     //CDC_ANC_CTL
	uint8_t adaptive;                         // Not used (reserved for future)
	uint8_t adaptive_gain_enable;             //CDC_ANC_B1_CTL
	uint8_t flex_enable;                      //CDC_ANC_B2_CTL
	uint8_t dmic_x2_enable;                   //CDC_ANC_B2_CTL
	uint8_t anc_lr_mix_output_channel;        // Decprecated, not used anymore since ACDB entry has both L and R entries
	int8_t  anc_ff_shift;                     //CDC_ANC_SHIFT // -4 -> 6
	int8_t  anc_fb_shift;                     //CDC_ANC_SHIFT // -4 -> 6
	uint8_t padding;
	int32_t anc_ff_coeff[15];                 //CDC_ANC_IIR_B3_CTL // IIR coeffs: A[2 - 8] B[1-8] - Q3.13 (signed)
	int32_t anc_gain;                         //iir coefficient calculation
	int32_t anc_ff_lpf_shift[2];              //CDC_ANC_LPF_B1_CTL // 0 -> 12
	int32_t anc_fb_coeff[13];                 //CDC_ANC_IIR_B3_CTL // IIR coeffs: A[2 - 7] B[1-7] - Q3.13 (signed)
	int32_t anc_gain_default;                 //not sure what this is for..
	int32_t anc_fb_lpf_shift[2];              //CDC_ANC_LPF_B1_CTL // 0 -> 12
	enum taiko_codec_anc_input_device input_device;  //CDC_ANC_CTL_ADC_DMIC_SEL & CDC_CONN_ANC_B1_CTL
	uint32_t  smlpf_shift;                      //CDC_ANC_SMLPF_CTL
	uint32_t  dcflt_shift;                      //CDC_ANC_DCFLT_CTL
	int32_t   adaptive_gain;                    //CDC_ANC_GAIN_CTL
} anc_taiko_acdb_cfg_t;

#define ANC_HW4 0x4000
/* ANC_HW4 id is associated with anc_tasha_acdb_cfg_t
   And Supports codec versions TASHA
 */
typedef struct anc_tasha_acdb_cfg_t {
	uint32_t ff_a_enable; //WCD9335_CDC_ANC0_CLK_RESET_CTL___FFA_CLK_EN && WCD9335_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN
	uint32_t ff_b_enable; //WCD9335_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN && WCD9335_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN
	uint32_t fb_enable;   //WCD9335_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN && WCD9335_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN

	uint32_t ff_a_in_enable;  //WCD9335_CDC_ANC0_MODE_1_CTL___FFA_IN_EN
	uint32_t ff_b_in_enable;  //WCD9335_CDC_ANC0_MODE_1_CTL___FFB_IN_EN
	uint32_t ff_a_input_device; //WCD9335_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_A_SEL // defined by enum codec_anc_input_device
	uint32_t ff_b_input_device; //WCD9335_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_B_SEL // defined by enum codec_anc_input_device
	uint32_t ff_out_enable;   //WCD9335_CDC_ANC0_MODE_1_CTL___ANC_OUT_EN
	uint32_t smlpf_enable; //WCD9335_CDC_ANC0_MODE_1_CTL___ANC_SMLPF_EN

	uint32_t ff_flex_enable;  //WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FFLE_EN
	uint32_t ff_a_gain_enable; //WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_A_EN
	uint32_t ff_b_gain_enable; //WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_B_EN
	uint32_t fb_gain_enable; //WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FBGAIN_EN
	uint32_t ff_a_dcflt_enable; //WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_A_EN
	uint32_t ff_b_dcflt_enable; //WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_B_EN
	uint32_t dmic_x2_ff_a_enable; //WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_A_SEL
	uint32_t dmic_x2_ff_b_enable; //WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_B_SEL

	int32_t anc_ff_a_shift; //WCD9335_CDC_ANC0_FF_SHIFT___FFA_SHIFT
	int32_t anc_ff_b_shift; //WCD9335_CDC_ANC0_FF_SHIFT___FFB_SHIFT
	int32_t anc_fb_shift; //WCD9335_CDC_ANC0_FB_SHIFT___FB_SHIFT

	int32_t anc_ff_a_den_coeff[7]; //WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR && WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF
	int32_t anc_ff_a_num_coeff[8]; //WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR && WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF
	int32_t anc_ff_a_gain_scale;
	int32_t anc_ff_a_gain_scale_default;
	int32_t anc_ff_a_gain; //WCD9335_CDC_ANC0_FF_A_GAIN_CTL___GAIN

	int32_t anc_ff_b_den_coeff[7]; //WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR && WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF
	int32_t anc_ff_b_num_coeff[8]; //WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR && WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF
	int32_t anc_ff_b_gain_scale;
	int32_t anc_ff_b_gain_scale_default;
	int32_t anc_ff_b_gain; // WCD9335_CDC_ANC0_FF_B_GAIN_CTL___GAIN

	int32_t anc_fb_den_coeff[7]; //WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR && WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF
	int32_t anc_fb_num_coeff[8]; //WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR && WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF
	int32_t anc_fb_gain_scale;
	int32_t anc_fb_gain_scale_default;
	int32_t anc_fb_gain; // WCD9335_CDC_ANC0_FB_GAIN_CTL___GAIN

	uint32_t anc_ff_a_lpf_shift[2]; //WCD9335_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S2 && WCD9335_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S1
	uint32_t anc_ff_b_lpf_shift[2]; //WCD9335_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S2 && WCD9335_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S1
	uint32_t anc_fb_lpf_shift[2]; //WCD9335_CDC_ANC0_LPF_FB_CTL___LPF_FB_S2 && WCD9335_CDC_ANC0_LPF_FB_CTL___LPF_FB_S1

	uint32_t ff_a_dcflt_shift; //WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL___A_SHIFT
	uint32_t ff_b_dcflt_shift; //WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL___B_SHIFT

	uint32_t smlpf_shift; //WCD9335_CDC_ANC0_SMLPF_CTL___SHIFT
} anc_tasha_acdb_cfg_t;

#define ANC_HW5 0x5000
/* ANC_HW5 id is associated with anc_wcd_hw5_acdb_cfg_t
   And Supports codec versions WCD9340 & above
 */
typedef struct anc_wcd_hw5_acdb_cfg_t {
   uint32_t anc_usecase; // Range: 0-1, 0: SmartLnQ Headset, 1: Handset, 2: Headset
   int32_t ff_a_mic_sensitivity; // unit dBVRMS/Pa
   int32_t ff_b_mic_sensitivity; // unit dBVRMS/Pa
   int32_t ff_a_fe_gain; // unit dB - not valid for SmartLnQ Headset since mic_sensitivity covers the mic amp/fe_gain
   int32_t ff_b_fe_gain; // unit dB - not valid for SmartLnQ Headset since mic_sensitivity covers the mic amp/fe_gain
   int32_t spkr_rcvr_sensitivity; // unit dBSPL/mW - spkr sensitivity for Headset usecases and rcvr for Handset usecase
   int32_t spkr_rcvr_impedance; // unit ohms - spkr impedance for Headset usecases and rcvr for Handset usecase
   int32_t spkr_rcvr_pa_gain; // unit dB - spkr pa gain for Headset usecases and rcvr for Handset usecase

   uint32_t ff_a_enable; //WCD9340_CDC_ANC0_CLK_RESET_CTL___FFA_CLK_EN && WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN
   uint32_t ff_b_enable; //WCD9340_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN && WCD9340_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN
   uint32_t fb_enable;   //WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN && WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN

   uint32_t ff_a_in_enable;  //WCD9340_CDC_ANC0_MODE_1_CTL___FFA_IN_EN
   uint32_t ff_b_in_enable;  //WCD9340_CDC_ANC0_MODE_1_CTL___FFB_IN_EN
   uint32_t ff_a_input_type; //WCD9340_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_A_SEL && WCD9340_CDC_ANC0_CLK_RESET_CTL___SMARTLNQ_A_EN // defined by enum codec_anc_input_type
   uint32_t ff_b_input_type; //WCD9340_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_B_SEL && WCD9340_CDC_ANC0_CLK_RESET_CTL___SMARTLNQ_B_EN// defined by enum codec_anc_input_type
   int32_t fb_mon; //WCD9340_CDC_ANC0_MODE_1_CTL___FB_ON_FBMON_IS_TRUE - 0 (legacy) : Ffa path IIR input goes to FB mon , 1(Adaptive Signal out) : FB path IIR input goes to FB mon
   uint32_t ff_out_enable;   //WCD9340_CDC_ANC0_MODE_1_CTL___ANC_OUT_EN
   uint32_t smlpf_enable; //WCD9340_CDC_ANC0_MODE_1_CTL___ANC_SMLPF_EN

   uint32_t ff_flex_enable;  //WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFLE_EN
   uint32_t ff_a_gain_enable; //WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_A_EN
   uint32_t ff_b_gain_enable; //WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_B_EN
   uint32_t fb_gain_enable; //WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FBGAIN_EN
   uint32_t ff_a_dcflt_enable; //WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_A_EN
   uint32_t ff_b_dcflt_enable; //WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_B_EN
   uint32_t dmic_x2_ff_a_enable; //WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_A_SEL
   uint32_t dmic_x2_ff_b_enable; //WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_B_SEL

   int32_t anc_ff_a_shift; //WCD9340_CDC_ANC0_FF_SHIFT___FFA_SHIFT
   int32_t anc_ff_b_shift; //WCD9340_CDC_ANC0_FF_SHIFT___FFB_SHIFT
   int32_t anc_fb_shift; //WCD9340_CDC_ANC0_FB_SHIFT___FB_SHIFT

   int32_t anc_ff_a_den_coeff[7]; //WCD9340_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR && WCD9340_CDC_ANC0_IIR_COEFF_2_CTL___COEF
   int32_t anc_ff_a_num_coeff[8]; //WCD9340_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR && WCD9340_CDC_ANC0_IIR_COEFF_2_CTL___COEF
   int32_t anc_ff_a_gain_scale;
   int32_t anc_ff_a_gain_scale_default;
   int32_t anc_ff_a_gain; //WCD9340_CDC_ANC0_FF_A_GAIN_CTL___GAIN

   int32_t anc_ff_b_den_coeff[7]; //WCD9340_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR && WCD9340_CDC_ANC0_IIR_COEFF_2_CTL___COEF
   int32_t anc_ff_b_num_coeff[8]; //WCD9340_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR && WCD9340_CDC_ANC0_IIR_COEFF_2_CTL___COEF
   int32_t anc_ff_b_gain_scale;
   int32_t anc_ff_b_gain_scale_default;
   int32_t anc_ff_b_gain; // WCD9340_CDC_ANC0_FF_B_GAIN_CTL___GAIN

   int32_t anc_fb_den_coeff[7]; //WCD9340_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR && WCD9340_CDC_ANC0_IIR_COEFF_2_CTL___COEF
   int32_t anc_fb_num_coeff[8]; //WCD9340_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR && WCD9340_CDC_ANC0_IIR_COEFF_2_CTL___COEF
   int32_t anc_fb_gain_scale;
   int32_t anc_fb_gain_scale_default;
   int32_t anc_fb_gain; // WCD9340_CDC_ANC0_FB_GAIN_CTL___GAIN

   uint32_t anc_ff_a_lpf_shift[2]; //WCD9340_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S2 && WCD9340_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S1
   uint32_t anc_ff_b_lpf_shift[2]; //WCD9340_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S2 && WCD9340_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S1
   uint32_t anc_fb_lpf_shift[2]; //WCD9340_CDC_ANC0_LPF_FB_CTL___LPF_FB_S2 && WCD9340_CDC_ANC0_LPF_FB_CTL___LPF_FB_S1

   uint32_t ff_a_dcflt_shift; //WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___A_SHIFT
   uint32_t ff_b_dcflt_shift; //WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___B_SHIFT

   uint32_t smlpf_shift; //WCD9340_CDC_ANC0_SMLPF_CTL___SHIFT

}anc_wcd_hw5_acdb_cfg_t;


//***************** Function Definitions
//
// Set to ADIE ANC
return_code_t convert_anc_acdb_to_reg(struct anc_reg_t *anc_reg_config_ptr, int8_t *anc_acdb_ptr, uint32_t input_size, codec_version_t codec_version);
// Out: anc_reg_ptr --> sequence of anc registers and values that need to be converted to acdb data, 32 bit word consists of "reg (msb 16 bits), mask(8 bits), val (8 bits)"
// In: anc_acdb_ptr --> pointer to acdb structure for both the ANC channels
// In: input_size --> length of the input buffer, i.e. length of buffer pointed by anc_acdb_ptr
// In: Codec Version


// Get from ADIE ANC
return_code_t get_anc_reg_read_sequence(struct anc_reg_t *anc_reg_ptr, uint32_t size, codec_version_t codec_version);
// Out: anc_reg_ptr --> sequence of anc registers to be queried for identifying the caibration on target, 32 bit word consists of "reg (msb 16 bits), direction_t(8 bits), val (8 bits valid only for write direction)"
// In: size --> size of anc_reg_ptr
// In: Codec Version

return_code_t convert_anc_reg_to_acdb(int8_t *anc_acdb_ptr, uint32_t anc_acdb_size, struct anc_reg_t *anc_reg_ptr, codec_version_t codec_version);
//Out: anc_acdb_ptr --> pointer to acdb structure for both the ANC channels
// In: size --> size of anc_acdb_ptr
// In: anc_reg_ptr --> sequence of anc registers and values that need to be converted to acdb data
// In: Codec Version

return_code_t convert_anc_acdb_to_acdb(int8_t *anc_acdb_out_ptr, uint32_t *filled_output_size, uint32_t output_size, int32_t out_struct_id, int8_t *anc_acdb_in_ptr, uint32_t input_size, int32_t in_struct_id);
// Out: int8_t *anc_acdb_out_ptr --> memory location for the output structure to be populated after conversion
// Out: uint32_t *filled_output_size --> total size filled in the memory location pointed by anc_acdb_out_ptr
// IN: uint32_t output_size --> size of the memory location pointed by anc_acdb_out_ptr, provided by the host
// IN: int32_t out_struct_id --> required output structure Supported ids: ANC_HW4, ANC_HW3
// IN: int8_t *anc_acdb_in_ptr --> memory location of the input structure that needs to be converted
// IN: uint32_t input_size --> size of the structure pointed by the address anc_acdb_in_ptr.
//                             eg: consider taiko structure is getting converted if the size is twice the anc_taiko_acdb_cfg_t
//                             It is understood that the input memory is pointing to 2 channels input rather than 1
// IN: int32_t in_struct_id --> input structure format Supported ids: ANC_HW3, ANC_HW4, in_struct_id and out_struct_id has to be different


// APIs for realigning the ANC coefficient pointer to point to LSB. These apis should be used before reading or writing of ANC coefficients
// Two parts of the api are as follows,
// wcd9335_get_anc_ptr_check_sequence_wcd9335 is the first step --> this provides the sequence in which data has to be read from wcd
// wcd9335_get_anc_ptr_correction_sequence is the second step --> this provides the sequence in needs to be writen to codec to re-align the internal coefficient pointer to LSB

return_code_t wcd9335_get_anc_ptr_check_sequence(struct anc_reg_t *anc_reg_out_ptr, uint32_t out_size, access_interface_t access_interface);
// Out: int8 *anc_acdb_out_ptr --> memory location for sequence that needs to be run to get the data from wcd
// IN: uint32_t out_size --> size of the structure pointed by the address anc_reg_out_ptr.
// IN: access_interface_t access_interface --> specifies type of interface used

return_code_t wcd9335_get_anc_ptr_correction_sequence(struct anc_reg_t *anc_reg_out_ptr, uint32_t out_size, struct anc_reg_t *anc_reg_in_ptr, uint32_t in_size, access_interface_t access_interface);
// Out: int8 *anc_acdb_out_ptr --> memory location for sequence that needs written to wcd for re-alignment
//                                 If the number of elements/2 , i.e. size>>1, is even - then the pointer in wcd was misalinged and this sequence will realign it
// IN: uint32_t out_size --> size of the structure pointed by the address anc_reg_out_ptr.
// IN: int8 *anc_acdb_out_ptr --> memory location that is the output of api wcd9335_get_anc_ptr_check_sequence_wcd9335 filled with read values from wcd
// IN: uint32_t in_size --> size of the structure pointed by the address anc_reg_in_ptr.
// IN: access_interface_t access_interface --> specifies type of interface used


void parse_codec_type(char *snd_card_name);

return_code_t anc_conversion(anc_reg_t *anc_reg_config_ptr,
		int *pANCCfg, int anc_version);
#endif//#ifndef ANC_MAP_API_H

