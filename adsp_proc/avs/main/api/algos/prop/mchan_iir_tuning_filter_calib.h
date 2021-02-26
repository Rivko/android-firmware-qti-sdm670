#ifndef MCHAN_IIR_TUNING_FILTER_CALIB_H
#define MCHAN_IIR_TUNING_FILTER_CALIB_H
/*==============================================================================
  @file mchan_iir_tuning_filter_calib.h
  @brief This file contains MCHAN_IIR_TUNING_FILTER API
==============================================================================*/

/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 03/21/17   pag     Added h2xml comments.
 ========================================================================== */


/** @h2xml_title1           {Multiple Channel IIR Tuning Filter}
    @h2xml_title_agile_rev  {Multiple Channel IIR Tuning Filter}
    @h2xml_title_date       {February 28, 2017} */

#include "mmdefs.h"

#define AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER              0x0001031F
/** @h2xmlm_module       {"AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER",
                          AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER}

    @h2xmlm_displayName  {"AUDIO MULTICHAN IIR"}
	@h2xmlm_description	 {ID of the Multiple Channel IIR Tuning Filter module.\n

    This module supports the following parameter IDs\n
     - #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE\n
     - #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN\n
     - #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS\n
     - #AUDPROC_PARAM_ID_ENABLE\n}

     @h2xmlm_toolPolicy              {Calibration; RTC}

    @{                   <-- Start of the Module -->
 */


/* ID of the Multichannel IIR Tuning Filter Enable parameters used by
    AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER.
 */
#define AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE        0x0001031C

/** Maximum number of channels for the multichannel IIR tuning filter. */
#define IIR_TUNING_FILTER_MAX_CHANNELS                 8

/* Structure for holding one channel type - IIR enable pair. */

/*
 This structure immediately follows the audproc_mchan_iir_enable_t
 structure.
 */
/** @h2xmlp_subStruct */
typedef struct audproc_channel_type_iir_enable_pair_t
{
   uint8_t channel_type;
   /**< @h2xmle_description  {Channel type for which the IIR enable is to be applied.\n\n
	PCM_CHANNEL_NULL=0\nPCM_CHANNEL_L=1\nPCM_CHANNEL_R=2\nPCM_CHANNEL_C=3\nPCM_CHANNEL_LS=4\nPCM_CHANNEL_RS=5\nPCM_CHANNEL_LFE=6\nPCM_CHANNEL_CB=7\nPCM_CHANNEL_LB=8\nPCM_CHANNEL_RB=9\nPCM_CHANNEL_TS=10\nPCM_CHANNEL_TFC=11\nPCM_CHANNEL_MS=12\nPCM_CHANNEL_FLC=13\nPCM_CHANNEL_FRC=14\nPCM_CHANNEL_RLC=15\nPCM_CHANNEL_RRC=16\nPCM_CHANNEL_LFE2=17\nPCM_CHANNEL_SL=18\nPCM_CHANNEL_SR=19\nPCM_CHANNEL_LVH=20\nPCM_CHANNEL_RVH=21\nPCM_CHANNEL_TC=22\nPCM_CHANNEL_TBL=23\nPCM_CHANNEL_TBR=24\nPCM_CHANNEL_TSL=25\nPCM_CHANNEL_TSR=26\nPCM_CHANNEL_TBC=27\nPCM_CHANNEL_BFC=28\nPCM_CHANNEL_BFL=29\nPCM_CHANNEL_BFR=30\nPCM_CHANNEL_LW=31\nPCM_CHANNEL_RW=32\nPCM_CHANNEL_LSD=33\nPCM_CHANNEL_RSD=34}

         @h2xmle_rangeList {	"PCM_CHANNEL_NULL"=0;"PCM_CHANNEL_L"=1;"PCM_CHANNEL_R"=2;"PCM_CHANNEL_C"=3;"PCM_CHANNEL_LS"=4;"PCM_CHANNEL_RS"=5;"PCM_CHANNEL_LFE"=6;"PCM_CHANNEL_CS"=7;"PCM_CHANNEL_LB"=8;"PCM_CHANNEL_RB"=9;"PCM_CHANNEL_TS"=10;"PCM_CHANNEL_CVH"=11;"PCM_CHANNEL_MS"=12;"PCM_CHANNEL_FLC"=13;"PCM_CHANNEL_FRC"=14;"PCM_CHANNEL_RLC"=15;"PCM_CHANNEL_RRC"=16;"PCM_CHANNEL_LFE2"=17;"PCM_CHANNEL_SL"=18;"PCM_CHANNEL_SR"=19;"PCM_CHANNEL_LVH"=20;"PCM_CHANNEL_RVH"=21;"PCM_CHANNEL_TC"=22;"PCM_CHANNEL_TBL"=23;"PCM_CHANNEL_TBR"=24;"PCM_CHANNEL_TSL"=25;"PCM_CHANNEL_TSR"=26;"PCM_CHANNEL_TBC"=27;"PCM_CHANNEL_BFC"=28;"PCM_CHANNEL_BFL"=29;"PCM_CHANNEL_BFR"=30;"PCM_CHANNEL_LW"=31;"PCM_CHANNEL_RW"=32;"PCM_CHANNEL_LSD"=33;"PCM_CHANNEL_RSD"=34	}
		@h2xmle_default      {0}
        @h2xmle_readOnly     {true}
                               */

   uint8_t reserved1;
   /**< @h2xmle_description  {Clients must set this field to 0.}
        @h2xmle_range    {0..0}
        @h2xmle_default	{0}                        */

   uint8_t reserved2;
   /**< @h2xmle_description  {Clients must set this field to 0.}
        @h2xmle_range    {0..0}
        @h2xmle_default	{0}                        */

   uint8_t reserved3;
   /**< @h2xmle_description  {Clients must set this field to 0.}
        @h2xmle_range    {0..0}
        @h2xmle_default	{0}                        */

   uint32_t enable_flag;
   /**< @h2xmle_description  {Specifies whether the channel IIR is enabled.}
        @h2xmle_rangeList    {"Disable"=0;
                              "Enable"=1}  */
}audproc_channel_type_iir_enable_pair_t
;

/* Structure for the multichannel IIR enable command */
typedef struct audproc_mchan_iir_enable_t audproc_mchan_iir_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE", AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE}
    @h2xmlp_description {ID of the Multichannel IIR Tuning Filter Enable parameters used by
    AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER.\n Payload of the AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE
 channel type/IIR enable pairs used by the Multiple Channel IIR Tuning
 Filter module.\n
 This structure immediately follows the audproc_mchan_iir_enable_t
 structure.\n}

 @h2xmlx_expandStructs	{false}
	*/
#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE
 parameters used by the Multiple Channel IIR Tuning Filter module.
 */
struct audproc_mchan_iir_enable_t
{
   uint32_t num_channels;
   /**< @h2xmle_description  {Number of channels for which enable flags are provided.\n IIR_TUNING_FILTER_MAX_CHANNELS=8}
        @h2xmle_range    {0..IIR_TUNING_FILTER_MAX_CHANNELS}
        @h2xmle_default	{1}
        @h2xmle_readOnly {true}  */

   audproc_channel_type_iir_enable_pair_t enable_flag_settings[IIR_TUNING_FILTER_MAX_CHANNELS];
   /**< @h2xmle_description  {enable_flag_settings}
        @h2xmle_default	{0} */

}
#include "adsp_end_pack.h"
;


/* ID of the Multiple Channel IIR Tuning Filter Pregain parameters used by
 */
#define AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN        0x0001031D

/* Structure for holding one channel type - IIR pregain pair. */

/** @h2xmlp_subStruct */

typedef struct audproc_channel_type_iir_pregain_pair_t
{
   uint8_t channel_type;
   /**< @h2xmle_description  {Channel type for which the IIR enable is to be applied.\n\n
	PCM_CHANNEL_NULL=0\nPCM_CHANNEL_L=1\nPCM_CHANNEL_R=2\nPCM_CHANNEL_C=3\nPCM_CHANNEL_LS=4\nPCM_CHANNEL_RS=5\nPCM_CHANNEL_LFE=6\nPCM_CHANNEL_CB=7\nPCM_CHANNEL_LB=8\nPCM_CHANNEL_RB=9\nPCM_CHANNEL_TS=10\nPCM_CHANNEL_TFC=11\nPCM_CHANNEL_MS=12\nPCM_CHANNEL_FLC=13\nPCM_CHANNEL_FRC=14\nPCM_CHANNEL_RLC=15\nPCM_CHANNEL_RRC=16\nPCM_CHANNEL_LFE2=17\nPCM_CHANNEL_SL=18\nPCM_CHANNEL_SR=19\nPCM_CHANNEL_LVH=20\nPCM_CHANNEL_RVH=21\nPCM_CHANNEL_TC=22\nPCM_CHANNEL_TBL=23\nPCM_CHANNEL_TBR=24\nPCM_CHANNEL_TSL=25\nPCM_CHANNEL_TSR=26\nPCM_CHANNEL_TBC=27\nPCM_CHANNEL_BFC=28\nPCM_CHANNEL_BFL=29\nPCM_CHANNEL_BFR=30\nPCM_CHANNEL_LW=31\nPCM_CHANNEL_RW=32\nPCM_CHANNEL_LSD=33\nPCM_CHANNEL_RSD=34}

         @h2xmle_rangeList {	"PCM_CHANNEL_NULL"=0;"PCM_CHANNEL_L"=1;"PCM_CHANNEL_R"=2;"PCM_CHANNEL_C"=3;"PCM_CHANNEL_LS"=4;"PCM_CHANNEL_RS"=5;"PCM_CHANNEL_LFE"=6;"PCM_CHANNEL_CS"=7;"PCM_CHANNEL_LB"=8;"PCM_CHANNEL_RB"=9;"PCM_CHANNEL_TS"=10;"PCM_CHANNEL_CVH"=11;"PCM_CHANNEL_MS"=12;"PCM_CHANNEL_FLC"=13;"PCM_CHANNEL_FRC"=14;"PCM_CHANNEL_RLC"=15;"PCM_CHANNEL_RRC"=16;"PCM_CHANNEL_LFE2"=17;"PCM_CHANNEL_SL"=18;"PCM_CHANNEL_SR"=19;"PCM_CHANNEL_LVH"=20;"PCM_CHANNEL_RVH"=21;"PCM_CHANNEL_TC"=22;"PCM_CHANNEL_TBL"=23;"PCM_CHANNEL_TBR"=24;"PCM_CHANNEL_TSL"=25;"PCM_CHANNEL_TSR"=26;"PCM_CHANNEL_TBC"=27;"PCM_CHANNEL_BFC"=28;"PCM_CHANNEL_BFL"=29;"PCM_CHANNEL_BFR"=30;"PCM_CHANNEL_LW"=31;"PCM_CHANNEL_RW"=32;"PCM_CHANNEL_LSD"=33;"PCM_CHANNEL_RSD"=34	}

	  @h2xmle_default      {0}
      @h2xmle_readOnly {true}                         */


   uint8_t reserved1;
   /**< @h2xmle_description  {Clients must set this field to 0.}
        @h2xmle_range    {0..0}
        @h2xmle_default	{0}                        */

   uint8_t reserved2;
   /**< @h2xmle_description  {Clients must set this field to 0.}
        @h2xmle_range    {0..0}
        @h2xmle_default	{0}                        */

   uint8_t reserved3;
   /**< @h2xmle_description  {Clients must set this field to 0.}
        @h2xmle_range    {0..0}
        @h2xmle_default	{0}                        */

   int32_t preGain;
   /**< @h2xmle_description  {Pregain of IIR for this channel (in Q27 format).}
        @h2xmle_default	{0x08000000}
		@h2xmle_dataFormat	{Q27} */

}audproc_channel_type_iir_pregain_pair_t

;


/* Structure for the multichannel IIR preGain command */
typedef struct audproc_mchan_iir_pregain_t audproc_mchan_iir_pregain_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN", AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN}
    @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN
 channel type/IIR pregain pairs used by the Multiple Channel IIR Tuning
 Filter module.\n
 This structure immediately follows the audproc_mchan_iir_pregain_t
 structure.\n}

  @h2xmlx_expandStructs	{false}	*/
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN
 parameters used by the Multiple Channel IIR Tuning Filter module.
 */
struct audproc_mchan_iir_pregain_t
{
   uint32_t num_channels;
   /**< @h2xmle_description  {Number of channels for which pregains are provided}
        @h2xmle_range    {0..IIR_TUNING_FILTER_MAX_CHANNELS}
        @h2xmle_default	{1}
        @h2xmle_readOnly {true}        */

   audproc_channel_type_iir_pregain_pair_t preGain_settings[IIR_TUNING_FILTER_MAX_CHANNELS];
   /**< @h2xmle_description  {Number of channels for which pregains are provided} */

}
#include "adsp_end_pack.h"
;
#define AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS     0x0001031E



/** @h2xmlp_subStruct */

typedef struct audproc_channel_type_iir_config_pair_t
{
   uint8_t channel_type;
   /**< @h2xmle_description  {Channel type for which the IIR enable is to be applied.\n\n
	PCM_CHANNEL_NULL=0\nPCM_CHANNEL_L=1\nPCM_CHANNEL_R=2\nPCM_CHANNEL_C=3\nPCM_CHANNEL_LS=4\nPCM_CHANNEL_RS=5\nPCM_CHANNEL_LFE=6\nPCM_CHANNEL_CB=7\nPCM_CHANNEL_LB=8\nPCM_CHANNEL_RB=9\nPCM_CHANNEL_TS=10\nPCM_CHANNEL_TFC=11\nPCM_CHANNEL_MS=12\nPCM_CHANNEL_FLC=13\nPCM_CHANNEL_FRC=14\nPCM_CHANNEL_RLC=15\nPCM_CHANNEL_RRC=16\nPCM_CHANNEL_LFE2=17\nPCM_CHANNEL_SL=18\nPCM_CHANNEL_SR=19\nPCM_CHANNEL_LVH=20\nPCM_CHANNEL_RVH=21\nPCM_CHANNEL_TC=22\nPCM_CHANNEL_TBL=23\nPCM_CHANNEL_TBR=24\nPCM_CHANNEL_TSL=25\nPCM_CHANNEL_TSR=26\nPCM_CHANNEL_TBC=27\nPCM_CHANNEL_BFC=28\nPCM_CHANNEL_BFL=29\nPCM_CHANNEL_BFR=30\nPCM_CHANNEL_LW=31\nPCM_CHANNEL_RW=32\nPCM_CHANNEL_LSD=33\nPCM_CHANNEL_RSD=34}

        @h2xmle_rangeList {	"PCM_CHANNEL_NULL"=0;"PCM_CHANNEL_L"=1;"PCM_CHANNEL_R"=2;"PCM_CHANNEL_C"=3;"PCM_CHANNEL_LS"=4;"PCM_CHANNEL_RS"=5;"PCM_CHANNEL_LFE"=6;"PCM_CHANNEL_CS"=7;"PCM_CHANNEL_LB"=8;"PCM_CHANNEL_RB"=9;"PCM_CHANNEL_TS"=10;"PCM_CHANNEL_CVH"=11;"PCM_CHANNEL_MS"=12;"PCM_CHANNEL_FLC"=13;"PCM_CHANNEL_FRC"=14;"PCM_CHANNEL_RLC"=15;"PCM_CHANNEL_RRC"=16;"PCM_CHANNEL_LFE2"=17;"PCM_CHANNEL_SL"=18;"PCM_CHANNEL_SR"=19;"PCM_CHANNEL_LVH"=20;"PCM_CHANNEL_RVH"=21;"PCM_CHANNEL_TC"=22;"PCM_CHANNEL_TBL"=23;"PCM_CHANNEL_TBR"=24;"PCM_CHANNEL_TSL"=25;"PCM_CHANNEL_TSR"=26;"PCM_CHANNEL_TBC"=27;"PCM_CHANNEL_BFC"=28;"PCM_CHANNEL_BFL"=29;"PCM_CHANNEL_BFR"=30;"PCM_CHANNEL_LW"=31;"PCM_CHANNEL_RW"=32;"PCM_CHANNEL_LSD"=33;"PCM_CHANNEL_RSD"=34	}
        @h2xmle_default      {0}
        @h2xmle_readOnly {true}                      */

   uint8_t reserved;
   /**< @h2xmle_description {Clients must set this field to 0.}
   @h2xmle_rangeList {"0"=0}
   @h2xmle_readOnly  {true}
        */

   uint16_t num_biquad_stages;
   /**< @h2xmle_description {Number of biquad-IIR bands.}
        @h2xmle_range  {0..20}
	@h2xmle_default {1}
        */

	int32_t ulFilterCoeffs[0];
   /**< @h2xmle_description  {ulFilterCoeffs}
        @h2xmlx_expandArray  {true}
        @h2xmle_default      {0x40000000}
	@h2xmle_policy {advanced}
        @h2xmle_variableArraySize  { "5*num_biquad_stages" } */

	int16_t sNumShiftFactor[0];
   /**< @h2xmle_description  {sNumShiftFactor}
        @h2xmlx_expandArray  {true}
        @h2xmle_default      {0x02}
	@h2xmle_policy {advanced}
        @h2xmle_variableArraySize  {"num_biquad_stages" } */

}audproc_channel_type_iir_config_pair_t

;


/* Structure for the multichannel IIR config params */
typedef struct audproc_mchan_iir_config_t audproc_mchan_iir_config_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS", AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS}
    @h2xmlp_description {Payload of the #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS
 parameters used by the Multichannel IIR Tuning Filter module.\nThis structure is followed by the multichannel IIR filter
    coefficients as follows\n
    - Channel type/configuration pairs - See the Payload format table.\n
    - Sequence of int32 ulFilterCoeffs - Five coefficients for each band, each
      in int32 format in the order of b0, b1, b2, a1, a2.\n
    - Sequence of int16 sNumShiftFactor - One int16 per band. The numerator
      shift factor is related to the Q factor of the filter coefficients b0,
      b1, b2.\n\n
    There must be one data sequence per channel.\n
    If the number of bands is odd, there must be an extra 16-bit padding by
    the end of the numerator shift factors. This extra 16-bit padding makes
    the entire structure 32-bit aligned. The padding bits must be all zeros.\n}

     @h2xmlx_expandStructs	{false}
	*/
#include "adsp_begin_pack.h"

/* Payload of the #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS
 parameters used by the Multichannel IIR Tuning Filter module.
 */
struct audproc_mchan_iir_config_t
{
   uint32_t num_channels;
   /**< @h2xmle_description  {Number of channels for which enable flags are provided.}
        @h2xmle_range    {0..IIR_TUNING_FILTER_MAX_CHANNELS}
        @h2xmle_default	{1}
        @h2xmle_readOnly {true}      */

   audproc_channel_type_iir_config_pair_t MultiChannelIIRConfig[0];
     /**< @h2xmle_description  {MultiChannelIIRConfig.}
          @h2xmle_variableArraySize {num_channels} */
}
#include "adsp_end_pack.h"
;


/**
   @h2xml_Select					{audproc_channel_type_iir_enable_pair_t}
   @h2xmlm_InsertParameter

*/

/**
   @h2xml_Select					{audproc_mchan_iir_pregain_t}
   @h2xmlm_InsertParameter

*/

/**
   @h2xml_Select					{audproc_mchan_iir_config_t}
   @h2xmlm_InsertParameter
*/

/**
   @h2xml_Select					{audproc_channel_type_iir_pregain_pair_t}
   @h2xmlm_InsertParameter
*/

/**
   @h2xml_Select					{audproc_channel_type_iir_config_pair_t}
   @h2xmlm_InsertParameter
*/

/**
   @h2xml_Select					{audproc_mchan_iir_enable_t}
   @h2xmlm_InsertParameter
*/



/** @}                   <-- End of the Module -->*/

#endif
